#include "SynthetizeImage.hpp"
#include "binarization.hpp"
#include "convertor.h"
#include "StructureDetection.hpp"
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
                                        vector<fontLetter> font, cv::Mat originalImage, int token)
{
  cv::Mat ret;
  Painter painter(background,blocks);
  painter.extractFont(font,originalImage);
  ret = painter.painting();
  painter.saveXML(token);
  return ret;
}

vector<cv::Rect> SynthetizeImage::createStandardBlock(cv::Mat background)
{
  return vector<cv::Rect>({cv::Rect(30,30,background.size().width-30,background.size().height-30)});
}

cv::Mat SynthetizeImage::composeImage(std::string fontPath, std::string backgroundPath, std::string text, int token)
{
  cv::Mat ret;
  cv::Mat background = cv::imread(backgroundPath);
  vector<cv::Rect> blocks = createStandardBlock(background);
  Painter painter(background,blocks);

  if (!text.empty()) {
    painter.setText(text);
  }

  painter.extractFont(fontPath);
  
  ret = painter.painting();
  painter.saveXML(token);
  return ret;
}

cv::Mat SynthetizeImage::SynthetizeAuto(cv::Mat img, int token)
{
  cv::Mat binarizedImage, background;
  vector<fontLetter> font;
  vector<cv::Rect> blocks;
  
  binarizedImage = binarization(img);
  background = extractBackground(img, binarizedImage);
  _background = background;
  font = extractFont(img, binarizedImage);
  blocks = extractBlock(img, binarizedImage);
  return createDocument(background, blocks, font, img, token);
}

string SynthetizeImage::saveFont(int token)
{
  ocr.saveFont(UPLOAD_DIR + QString::number(token) + ".of");
  return to_string(token) + ".of";
}

string SynthetizeImage::saveBackground(int token)
{
  cv::imwrite(UPLOAD_DIR + to_string(token) + "_background.png",_background);
  return to_string(token) + "_background.png";
}

