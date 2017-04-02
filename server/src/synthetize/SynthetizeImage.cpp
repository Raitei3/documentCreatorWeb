#include "SynthetizeImage.hpp"
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

void SynthetizeImage::extractBackground(cv::mat originalImage, cv::Mat binarizedImage)
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

void SynthetizeImage::extractFont()
{
  ocr.setParameters("/usr/share/tesseract-ocr/", "eng");
  ocr.init(Convertor::getQImage(image), Convertor::getQImage(binarizedImage));
  font = ocr.getFinalFont();
}

void SynthetizeImage::extractBlock()
{
    
  cv::Mat distanceMap = structureDetection::getDistanceMap(image, binarizedImage);
  characterHeight = structureDetection::getCharacterHeight(binarizedImage);
  blocksImage = structureDetection::getBlocks(distanceMap, characterHeight*1.5);
}

void SynthetizeImage::createDocument()
{
  Painter painter(background,blocksImage, characterHeight);
  painter.extractFont(font,image);
  result = painter.painting();
}

cv::Rect SynthetizeImage::createStandardBlock(cv::Mat background)
{
  return cv::Rect(30,30,background.size().width-30,background.size().height-30);
}

cv::Mat SynthetizeImage::composeImage(std::string fontPath, std::string backgroundPath, std::string text)
{
  background = cv::imread(backgroundPath);
  blocksImage.push_back(createStandardBlock(background));
  Painter painter(background,blocksImage, characterHeight);

  if (!text.empty()) {
    painter.setText(text);
  }

  painter.extractFont(fontPath);
  result = painter.painting();

  return result;
}

cv::Mat SynthetizeImage::SynthetizeAuto(cv::Mat img)
{
  image = img;
  binarization();
  extractBackground();
  extractFont();
  extractBlock();
  createDocument();
  return result;
}
