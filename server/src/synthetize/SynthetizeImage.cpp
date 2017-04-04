#include "SynthetizeImage.hpp"

#include <ctime>

#include "binarization.hpp"
#include "convertor.h"
#include "StructureDetection.hpp"
#include "Painter.hpp"
#include "BackgroundReconstruction.hpp"

using namespace std;

cv::Mat SynthetizeImage::binarization(cv::Mat originalImage)
{
  cv::Mat binarizedImage;
  Binarization::preProcess(originalImage, binarizedImage, 12);
  Binarization::NiblackSauvolaWolfJolion(binarizedImage, binarizedImage,
                                         Binarization::WOLFJOLION, 128, 40, 40, 0.34);
  Binarization::postProcess(binarizedImage, binarizedImage, 0.9, 7);
  return binarizedImage;
}

cv::Mat SynthetizeImage::extractBackground(cv::Mat originalImage, cv::Mat binarizedImage)
{
  QImage binarized;
  cv::Mat output;
  BackgroundReconstruction back;

  binarized = Convertor::getQImage(binarizedImage);
  QImage origin = Convertor::getQImage(originalImage);
  back.setOriginalImage(origin);
  back.setBinarizedImage(binarized);
  back.process();
  QImage res = back.getResultImage();

  return Convertor::getCvMat(back.getResultImage());
}

vector<fontLetter> SynthetizeImage::extractFont(cv::Mat originalImage, cv::Mat binarizedImage,
                                                string language, string tesseractDir)
{
  ocr.setParameters(QString::fromStdString(tesseractDir), QString::fromStdString(language));
  ocr.init(Convertor::getQImage(originalImage), Convertor::getQImage(binarizedImage));
  return ocr.getFinalFont();
}

vector<cv::Rect> SynthetizeImage::extractBlock(cv::Mat originalImage, cv::Mat binarizedImage)
{
  cv::Mat distanceMap = structureDetection::getDistanceMap(originalImage, binarizedImage);
  int characterHeight = structureDetection::getCharacterHeight(binarizedImage);
  return structureDetection::getBlocks(distanceMap, characterHeight*1.5);
}

cv::Mat SynthetizeImage::createDocument(cv::Mat background, vector<cv::Rect> blocks,
                                        vector<fontLetter> font, cv::Mat originalImage)
{
  Painter painter(background,blocks);
  painter.extractFont(font,originalImage);
  return painter.painting();
}

vector<cv::Rect> SynthetizeImage::createStandardBlock(cv::Mat background)
{
  return vector<cv::Rect>({cv::Rect(30,30,background.size().width-30,background.size().height-30)});
}

cv::Mat SynthetizeImage::composeImage(std::string fontPath, std::string backgroundPath, std::string text)
{
  cv::Mat background = cv::imread(backgroundPath);
  vector<cv::Rect> blocks = createStandardBlock(background);
  Painter painter(background,blocks);

  if (!text.empty()) {
    painter.setText(text);
  }

  painter.extractFont(fontPath);
  return painter.painting();
}

cv::Mat SynthetizeImage::SynthetizeAuto(cv::Mat img)
{
  cv::Mat binarizedImage;
  cv::Mat ret;
  vector<fontLetter> font;
  vector<cv::Rect> blocks;
  clock_t t1,t2,t3,t4,t5,t6;

  t1=clock();
  binarizedImage = binarization(img);
  t2=clock();
  background = extractBackground(img, binarizedImage);
  t3=clock();
  font = extractFont(img, binarizedImage);
  t4=clock();
  blocks = extractBlock(img, binarizedImage);
  t5=clock();
  ret = createDocument(background, blocks, font, img);
  t6=clock();

  cerr << "binarization : " << 1000*(t2-t1)/CLOCKS_PER_SEC << "ms" << endl;
  cerr << "extraction fond : " << 1000*(t3-t2)/CLOCKS_PER_SEC << "ms" << endl;
  cerr << "extraction police : " << 1000*(t4-t3)/CLOCKS_PER_SEC << "ms" << endl;
  cerr << "exctraction blocks : " << 1000*(t5-t4)/CLOCKS_PER_SEC << "ms" << endl;
  cerr << "painter : " << 1000*(t6-t5)/CLOCKS_PER_SEC << "ms" << endl;
  ocr.saveFont("font.of");
  return ret;
}

string SynthetizeImage::saveFont(int token)
{
  ocr.saveFont(UPLOAD_DIR + QString::number(token) + ".of");
  return to_string(token) + ".of";
}

string SynthetizeImage::saveBackground(int token)
{
  cv::imwrite(UPLOAD_DIR + to_string(token) + "_background.png",background);
  return to_string(token) + "_background.png";
}
