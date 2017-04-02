#include "SynthetizeImage.hpp"
#include "binarization.hpp"
#include "convertor.h"
#include "StructureDetection.hpp"
#include "Painter.hpp"
#include "BackgroundReconstruction.hpp"


void SynthetizeImage::binarization(){
  Binarization::preProcess(image, binarizedImage, 12);
  Binarization::NiblackSauvolaWolfJolion(binarizedImage, binarizedImage, Binarization::WOLFJOLION, 128, 40, 40, 0.34);
  Binarization::postProcess(binarizedImage, binarizedImage, 0.9, 7);
}

void SynthetizeImage::extractBackground(){
  QImage binarized;
  cv::Mat output;

  binarized=Convertor::getQImage(binarizedImage);
  QImage origin = Convertor::getQImage(image);
  BackgroundReconstruction back;
  back.setOriginalImage(origin);
  back.setBinarizedImage(binarized);
  back.process();
  QImage res = back.getResultImage();


  background = Convertor::getCvMat(back.getResultImage());
  cv::imwrite("data/backgroundServeur.png",background);

}

void SynthetizeImage::extractFont(){
    ocr.setParameters("/usr/share/tesseract-ocr/","eng");
    ocr.init(Convertor::getQImage(image),Convertor::getQImage(binarizedImage));
    font = ocr.getFinalFont();
    ocr.saveFont("data/test2.of");
}

void SynthetizeImage::extractBlock(){
    
    cv::Mat distanceMap = structureDetection::getDistanceMap( image,binarizedImage);
    characterHeight = structureDetection::getCharacterHeight(binarizedImage);
    blocksImage = structureDetection::getBlocks(distanceMap,characterHeight*1.5);
}

void SynthetizeImage::createDocument(){
    Painter painter(background,blocksImage,characterHeight);
    painter.extractFont(font,image);
    //painter.extractFont("data/test2.of");
    result = painter.painting();
}

cv::Rect SynthetizeImage::createStandardBlock(cv::Mat background){
  return cv::Rect(30,30,background.size().width-30,background.size().height-30);
}

cv::Mat SynthetizeImage::composeImage(std::string fontPath, std::string backgroundPath, std::string text ){

  background = cv::imread(backgroundPath);
  blocksImage.push_back(createStandardBlock(background));
  Painter painter(background,blocksImage,characterHeight);

  if (text!="") {
    painter.setText(text);
  }

  painter.extractFont(fontPath);
  result = painter.painting();

  cv::imwrite("data/testCompose.png",result);
  return result;

}

cv::Mat SynthetizeImage::SynthetizeAuto(cv::Mat img){

  image = img;

  binarization();
  extractBackground();
  extractFont();
  extractBlock();
  createDocument();
  return result;


}
