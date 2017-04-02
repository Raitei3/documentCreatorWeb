#include "OCR.hpp"

#include <cstdlib>
#include <fstream>
#include <iostream>

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QMouseEvent>
#include <QTableView>
#include <QTextStream>

#include <convertor.h>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "baseline.hpp"
#include "binarization.hpp"

//#include "tesseract_config.h" //TESSERACT_TESSDATA_PARENT_DIR;

//#define WRITE_BASELINES true

static const int IMG_WIDTH = 500;
static const int IMG_HEIGHT = 680;
static const cv::Scalar SELECTED_BLUE = cv::Scalar(255, 0, 0);


OCR::OCR(QObject *parent) :
  QObject(parent),
  m_table(nullptr),
  m_currentIndex(0)
{}

OCR::~OCR()
{}

void OCR::setParameters(const QString &tessdataParentDir, const QString &language)
{
  m_tessdataParentDir = tessdataParentDir;
  m_language = language;
}

void OCR::init(const QImage &ori, const QImage &bin)
{
  setOriginalImage(ori);
  setBinarizedImage(bin);
  process();
  updateAlphabet();
}

void OCR::setOriginalImage(const QImage &img)
{
  // Initializa the image and launch the process
  m_originalImg = img;
}

void OCR::setBinarizedImage(const QImage &img)
{
  m_binarizedImg = img;
}

cv::Mat OCR::getLetterViewFromMask(const cv::Mat &original, const cv::Mat &mask) const
{
  // We extract the image
  cv::Mat mask_tmp;
  cv::threshold(mask, mask_tmp, 20, 1, cv::THRESH_BINARY_INV);

  cv::Mat letter;
  original.copyTo(letter);
  cv::add(letter, 100, letter);
  //cv::threshold(letter, letter, 237, 237, cv::THRESH_TRUNC);

  original.copyTo(letter, mask_tmp);
  return letter;
}

cv::Mat OCR::getImageFromMask(const cv::Mat &original, const cv::Mat &mask, int background_value) const
{
  // We extract the image
  cv::Mat mask_tmp;
  cv::threshold(mask, mask_tmp, 20, 1, cv::THRESH_BINARY_INV);

  cv::Mat letter;
  letter = cv::Mat::ones(mask_tmp.rows, mask_tmp.cols, original.type());
  letter.setTo(cv::Scalar(background_value, background_value, background_value));

  original.copyTo(letter, mask_tmp);

  return letter;
}

#include <iostream>//DEBUG

#include <clocale>

void OCR::process()
{
  cv::Mat src(Convertor::getCvMat(m_binarizedImg)), original(Convertor::getCvMat(m_originalImg)), tmp;
  cvtColor(src, tmp, CV_RGB2GRAY);

  std::vector<cv::Vec4i> baselines_tmp = m_baselines;
  Baseline::computeBaselines(tmp, baselines_tmp);

  /* Tesseract processing */

  tesseract::TessBaseAPI tess;
  tess.SetVariable("tessedit_char_whitelist", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

  const QByteArray tessdataParentDirBA = m_tessdataParentDir.toLatin1();
  const char *tessdataParentDir = tessdataParentDirBA.constData();
  //B:Warning: tessdataParentDir must end with "/"
  const QByteArray languageBA = m_language.toLatin1();
  const char *language = languageBA.constData();

  std::cerr<<"ocr tessdataParentDir="<<tessdataParentDir<<"\n";
  std::cerr<<"ocr language="<<language<<"\n";

#ifndef TESSERACT_VERSION

  const std::string lc = setlocale(LC_NUMERIC, NULL);
  const std::string newLocale = "C";
  if (lc != newLocale) {
    setlocale(LC_NUMERIC, newLocale.c_str());
    const std::string newLocaleSet = setlocale(LC_NUMERIC, NULL);
    std::cerr<<"Warning: changing LC_NUMERIC from "<<lc<<" to "<<newLocaleSet<<" for Tesseract\n";
    /*
      On Ubuntu 14.04.1, with tesseract 3.03.02-3,
      if we do not change the LC_NUMERIC to C and let it to fr_FR.UTF-8 for example,
      tess.Init() fails and prints the following error:

      Error: Illegal min or max specification!
      "Fatal error encountered!" == NULL:Error:Assert failed:in file globaloc.cpp, line 75

      On Ubuntu 16.04, tesseract 3.04, with same locale, there is no such error.

      It seems that tesseract 3.03.02-3 does not define TESSERACT_VERSION
    */
  }

#endif //TESSERACT_VERSION

  const int tessInitFailed = tess.Init(tessdataParentDir, language, tesseract::OEM_DEFAULT);  


  if (! tessInitFailed) {
    
    tess.SetPageSegMode(tesseract::PSM_SPARSE_TEXT);
    
    tess.SetImage((uchar*)tmp.data, tmp.cols, tmp.rows, 1, tmp.cols);
    tess.GetUTF8Text();
    
    tesseract::ResultIterator* ri = tess.GetIterator();

    cv::Mat base, base2;
    original.copyTo(base);
  
#if WRITE_BASELINES
    {
      for(int i=0; i<(int)baselines_tmp.size(); ++i) {
	cv::Vec4i l = baselines_tmp[i];
	double angle = atan2(l[1] - l[3], l[0] - l[2]);
	if(fabs(angle) > 3)
	  cv::line(base, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(50, 255, 50));
      }
      
      cv::imwrite("myBaselinesLol.png", base);
      base = cv::Mat::zeros(src.rows, src.cols, src.type());
      original.copyTo(base);
    }
#endif //WRITE_BASELINES
    
    
    // For each detected Symbol
    if (ri != 0) {
      
      do {
	
	// We get the corresponding guessed character
	const char* symbol = ri->GetUTF8Text(tesseract::RIL_SYMBOL);
	
	if (symbol != 0 && strcmp("\"", symbol) !=0) {
	  
#if WRITE_BASELINES
	  if (ri->IsAtBeginningOf(tesseract::RIL_TEXTLINE)) {
	    int x1, y1, x2, y2;
	    ri->Baseline(tesseract::RIL_TEXTLINE, &x1, &y1, &x2, &y2);
	    cv::line(base, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(255, 0, 0));
	  }
#endif //WRITE_BASELINES
	  
	  // Bounding box computation
	  int x1, y1, x2, y2;
	  ri->BoundingBox(tesseract::RIL_SYMBOL, &x1, &y1, &x2, &y2);
	  cv::Rect r = cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2));
	  
	  // Tesseract baseline extraction
	  ri->Baseline(tesseract::RIL_SYMBOL, &x1, &y1, &x2, &y2);
	  
	  // Tesseract confidence determination
	  float conf = ri->Confidence(tesseract::RIL_SYMBOL);
	  
	  // We extract the image
	  //cv::Mat mask = src(r);
	  cv::Mat mask;
	  original(r).copyTo(mask);
	  
	  // Then the object is created
	  fontLetter fl;
	  fl.binarization_step = Binarization::binarize(mask, mask);
	  fl.mask =  mask;
	  fl.label = symbol;
	  
	  const int b2 = Baseline::getBaseline(r, baselines_tmp);
	  fl.baseline2 = b2 - r.y;
	  fl.baseline = (int)((y1 + y2)/2 - r.y);
	  
	  fl.GT_baseline = (int)((fl.baseline2 + fl.baseline) / 2);
	  fl.confidence = conf;
	  fl.rect = r;
	  fl.checked = false;

	  // And pushed to the list
	  m_font.push_back(fl);
	  
	}
	delete[] symbol;
	
      } while((ri->Next(tesseract::RIL_SYMBOL)));

      if (! m_font.empty())
	m_currentLetter = m_font[0];
    }

#if WRITE_BASELINES
    cv::imwrite("tessBaselines.png", base);
#endif //WRITE_BASELINES

    m_currentIndex = 0;

    //  updateView();
  }
}

void OCR::updateAlphabet()
{
  m_alphabet.clear();

  // We fill the alphabet
  for(int i=0; i<(int)m_font.size(); ++i) {
    fontLetter &f = m_font[i];
    bool found = false;

    for(int j=0; j<(int)m_alphabet.size(); ++j) {
      const int ind = m_alphabet[j].first;
      assert(ind>=0 && ind<m_font.size());
      const fontLetter &fl = m_font[ind];

      // If the label is already in table, we increase its instance number
      if(f.label == fl.label) {
	m_alphabet[j].second++;
	found = true;
	break;
      }
    }
    // Else, we create a new instance of this label
    if(!found)
      m_alphabet.push_back(std::pair<int, int>(i, 1));
  }

  // We sort the table according to their frequency in the text
  sort(m_alphabet.begin(), m_alphabet.end(), [](const std::pair<int, int>& c1, const std::pair<int, int>& c2) {
      return c1.second > c2.second;
    });
}

QColor OCR::getConfidenceColor(float conf) const
{
  // Returns a color from a confidence value (between 0-100)
  QColor color(230, 230, 230);
  if(conf < 60)
    color = QColor(255, 215, 215);
  else if(conf < 80)
    color = QColor(250, 250, 250);
  else if(conf >= 85 && conf < 100)
    color = QColor(215, 255, 215);
  else if(conf >= 100)
    color = QColor(100, 255, 100);

  return color;
}


std::vector<int> OCR::getSimilarLetters(const fontLetter &fl) const
{
  /* Returns a sorted vector of the symbols with the same label than fl
   * Returns also the related index of each letter
   * The letters are sorted by confidence of the guessed label */

  // Get symbols with same label
  std::vector<std::pair<int, int>> list;

  const size_t sz = m_font.size();
  for (int i=0; i<(int)sz; ++i) {
    const fontLetter f = m_font[i];
    if(f.label == fl.label) {
      list.push_back(std::pair<int, int>(i, f.confidence));
    }
  }

  // Sort them by decreasing confidence
  sort(list.begin(), list.end(), [](std::pair<int, int> c1, std::pair<int, int> c2) {
      return c1.second > c2.second;
    });


  // We select only the n-firsts (where n is given by the user)
  const size_t size = std::min((int)list.size(), m_maxNumberOfSymbols);
  std::vector<int> croplist(size);
  for(size_t i=0; i<size; ++i)
    croplist[i] = list[i].first;
  return croplist;
}


std::vector<fontLetter> OCR::getFinalFont() const
{
  int spacing_w =0, spacing_h = 0;
  int sum = 0;
  std::vector<fontLetter> finalFont;
  for (int i=0; i<(int)m_alphabet.size(); ++i) {

    const int ind = m_alphabet[i].first;
    const fontLetter &fl = m_font[ind];

    const std::vector<int> sim = getSimilarLetters(fl);
    for(int ind2 : sim) {

      const fontLetter &fls = m_font[ind2];
      finalFont.push_back(fls);
      spacing_w += fls.rect.width;
      spacing_h += fls.rect.height;
      ++sum;
    }
  }

  spacing_w /= sum;
  spacing_h /= sum;

  fontLetter space;
  space.binarization_step = 0;
  space.mask =  cv::Mat::ones(spacing_h, spacing_w, CV_8U);
  space.label = " ";
  space.baseline2 = spacing_h;
  space.baseline = spacing_h;

  space.GT_baseline = spacing_h;
  space.confidence = 100;
  space.rect = cv::Rect(0, 0, spacing_w, spacing_h);
  space.checked = false;

  finalFont.insert(finalFont.begin(), space);

  return finalFont;
}

void OCR::rebinarizeCurrentLetter()
{
  // Re-binarize the image of the current symbol
  cv::Mat thumb = Convertor::getCvMat(m_originalImg);
  thumb = thumb(m_currentLetter.rect);
  cv::cvtColor(thumb, thumb, CV_RGB2GRAY);

  cv::Mat img_bin(thumb.rows, thumb.cols, CV_8U);

  cv::threshold(thumb, img_bin, m_currentLetter.binarization_step, 255, CV_THRESH_BINARY);

  // Holes filling + noise reduction
  if(m_currentLetter.checked)
    cv::medianBlur(img_bin, img_bin, 5);
  m_currentLetter.mask = img_bin;
//  updateView();

}

/*
void OCR::on_pushButton_3_clicked()
{
  float error1 =0;
  float error2 =0;
  for (const fontLetter &f : m_validatedFont) {
    error1 += std::abs(f.baseline - f.GT_baseline);
    error2 += std::abs(f.baseline2 - f.GT_baseline);
  }

  error1 /= m_validatedFont.size();
  error2 /= m_validatedFont.size();

  //B:useless ???
}
*/

QString OCR::saveFont()
{
  QString filters("font files (*.of)");
  QString filename = QFileDialog::getSaveFileName(nullptr, "Save Font", QDir::currentPath(), filters);
  writeFont(filename, getFinalFont());

  return filename;
}

void OCR::saveFont(const QString &filename)
{
  writeFont(filename, getFinalFont());
}

void OCR::writeFont(const QString &filename,
			  const std::vector<fontLetter> &finalFont) const
{

  QFile file(filename);
  if(file.open(QIODevice::ReadWrite)) {

    const QString fontName = QFileInfo(filename).completeBaseName();

    const cv::Mat thumb = Convertor::getCvMat(m_originalImg);

    QTextStream stream(&file);

    stream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
    stream << "<font name=\"" << fontName << "\">" << endl;

    std::string currentLabel = "papayouki"; //B:TODO: WTF?
    int picture_id = 1;

    const size_t sz = finalFont.size();
    for(int letter=0; letter<(int)sz; ++letter) {
      const fontLetter &fl = finalFont[letter];

      if(currentLabel.compare("\"") == 0)
	continue;

      if(currentLabel.compare(fl.label) != 0) {
	if(currentLabel.compare("papayouki") != 0)
	  stream << "</letter>" << endl;

	stream << "<letter char=\"" << QString::fromStdString(fl.label) << "\">" << endl <<
	  "<anchor>" << endl <<
	  "<upLine>0</upLine>" << endl <<
	  "<baseLine>" << (int)(((double)fl.baseline / (double)fl.rect.height) * 100) << "</baseLine>" << endl <<
	  "<leftLine>0</leftLine>" << endl <<
	  "<rightLine>100</rightLine>" << endl <<
	  "</anchor>" << endl;

	currentLabel = fl.label;
	picture_id = 1;
      }

      stream << "<picture id=\"" << picture_id << "\">" << endl <<
	"<imageData>" << endl <<
	"<width>" << fl.rect.width << "</width>" << endl <<
	"<height>" << fl.rect.height << "</height>" << endl <<
	"<format>5</format>" << endl << "<degradationlevel>0</degradationlevel>" << endl <<
	"<data>" << endl;


      cv::Mat image = getImageFromMask(thumb(fl.rect), fl.mask, 255);
      for(int i=0; i<image.rows; ++i) {

	cv::Vec3b *p = image.ptr<cv::Vec3b>(i);
	for(int j=0; j<image.cols; ++j) {

	  const cv::Vec3b pix = p[j];
	  unsigned char opacity;
	  // if color is white make it transparent
	  if(letter == 0 || (pix[0] == 255 && pix[1] == 255 && pix[2] == 255))
	    opacity = 0;
	  else
	    opacity = 255;

	  // transforme RGB to ARGB
	  unsigned int v = (opacity<<24)|(pix[0]<<16)|(pix[1]<<8)|(pix[2]);

	  stream << v;
	  if(j!= image.cols -1 || i != image.rows -1)
	    stream << ",";
	}
      }

      stream << "</data>" << endl << "</imageData>" << endl << "</picture>" << endl;

      ++picture_id;
    }

    stream << "</font>" << endl;

    file.close();
    std::cout << "font saved" << std::endl;
  }
}
