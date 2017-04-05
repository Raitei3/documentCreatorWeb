#include "LoadLetter.hpp"

#include <QFile>
#include <QXmlStreamReader>

#include "convertor.h"

using namespace std;

/* Prend en parametre le chemin d'un fichier de police (format .of)
 * renvoi la structure utilisée dans l'affichage pour représenter une police
 *  et contenant la police du fichier d'entrée
 */
map<string,vector<fontLetter> >  LoadLetter::fromFile(const string &path)
{
  map<string,vector<fontLetter>> fontMap;
  QFile font(QString::fromStdString(path));
  const bool ok = font.open( QFile::ReadOnly );

  if (!ok) {
    std::cerr<<"Warning: unable to open font file: \n";
  }

  QXmlStreamReader reader(&font);
  int width=0;
  int height=0;
  QString s;
  string s2;
  int baseline = 0;
  int rightLine = 100;
  fontLetter f;

  while(!reader.atEnd()){
    QXmlStreamReader::TokenType token = reader.readNext();
    if (token == QXmlStreamReader::StartElement) {
      if(reader.name()=="letter"){
        s = reader.attributes().value("char").toString();
        s2 = s.toStdString();
      }
      else if (reader.name() == "width") {
        reader.readNext();
        width = reader.text().toString().toInt();
      }
      else if (reader.name() == "height") {
        reader.readNext();
        height = reader.text().toString().toInt();
      }
      else if (reader.name()=="baseLine") {
        reader.readNext();
        baseline = reader.text().toString().toInt();
      }
      else if (reader.name()=="rightLine"){
        reader.readNext();
        rightLine = reader.text().toString().toInt();
      }
      else if (reader.name() == "data") {
        reader.readNext();
        QString data = reader.text().toString();
        cv::Mat mat = extractImage(data,width,height);
        f.mask = mat;
        f.baseline = baseline;
        f.rightLine = rightLine;
        fontMap[s2].push_back(f);
      }
    }
  }

  if(reader.hasError())
    cerr<<"Error at line "<<reader.lineNumber()<<" : "<<reader.errorString().toStdString()<<endl;
  return fontMap;
}

map<string,vector<fontLetter> > LoadLetter::fromVector(const vector<fontLetter> &vec,
                                                       const cv::Mat &background)
{
  map<string,vector<fontLetter>> font;
  string s;

  for(auto it=vec.begin();it != vec.end();it++){
    fontLetter l=*it;
    s=l.label;
    l.mask=getImageFromMask(background(l.rect), l.mask, 255);
    font[s].push_back(l);
    font[s].push_back(l);
    if(font[s].size()==1){
      font[s].back().baseline=((double)l.baseline/(double)l.rect.height) *100;
    }
    else{
      font[s].back().baseline=font[s].front().baseline;
    }
  }
  return font;
}

cv::Mat LoadLetter::extractImage(QString str, int width, int heigth){
  QImage ret=QImage(width,heigth,QImage::Format_ARGB32);
  QStringList list=str.split(',');
  auto it=list.begin();
  for(int j=0; j<heigth; j++){
    QRgb* d=(QRgb*)ret.scanLine(j);
    for(int i=0; i<width; i++,it++){
      d[i]=it->toUInt();
    }
  }
  cv::Mat mat=Convertor::getCvMat(ret);
  return mat;
}

cv::Mat LoadLetter::getImageFromMask(const cv::Mat &original, const cv::Mat &mask, int background_value)
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
