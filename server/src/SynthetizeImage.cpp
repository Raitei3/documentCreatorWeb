#include "SynthetizeImage.hpp"
#include "binarization.hpp"
#include "convertor.h"
#include "StructureDetection.hpp"
#include "Painter.hpp"
#include "synthetizeTest.hpp"
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
    characterHeight = structureDetection::getCharacterHeight(binarizedImage);
    cv::Mat distanceMap = structureDetection::getDistanceMap( image,binarizedImage);
    blocksImage = structureDetection::getBlocks(distanceMap,characterHeight);
}

void SynthetizeImage::createDocument(){
    Painter painter(background,blocksImage,characterHeight);
    painter.extractFont(font);
    //painter.extractFont("data/test2.of");
    result = painter.painting();
}

cv::Rect SynthetizeImage::createStandardBlock(cv::Mat background){
  return cv::Rect(30,30,background.size().width-30,background.size().height-30);
}


bool SynthetizeImage::getPage(HttpRequest* request, HttpResponse *response)
{
  const char* url = request->getUrl();

  std::cout << url << std::endl;

  if (!strcmp(url,"composeImage.txt")) {


    std::string fontPath;
    std::string backgroundPath;
    std::string text;
    request->getParameter("font",fontPath);
    request->getParameter("background",backgroundPath);
    request->getParameter("text",text);

    fontPath = "data/"+fontPath;
    backgroundPath= "data/"+backgroundPath;


    characterHeight =50;
    background = cv::imread(backgroundPath);
    


    std::cout << fontPath << std::endl;
    //std::cout << backgroundPath << std::endl;
    //std::cout << text << std::endl;

  }

  else{
    std::string tokenParam;
    request->getParameter("token", tokenParam);
    int token = stoi(tokenParam);
    int sessionIndex = getActiveSessionFromToken(token);
    std::cout << tokenParam << std::endl;

    if(sessionIndex != -1)
    {


      image = activeSessions.at(sessionIndex)->getImage()->getMat();
      //cv::Mat image=cv::imread("data/image/Bmt_res2812_002.png");

      binarization();
      cv::imwrite("data/binarizedserveur.png",binarizedImage);
      extractFont();
      extractBackground();
      //cv::imwrite("data/backgroundServeur.png",background);
      extractBlock();
      createDocument();

      activeSessions.at(sessionIndex)->getImage()->setMat(result);
      activeSessions.at(sessionIndex)->saveDisplayedImage(UPLOAD_DIR);
      myUploadRepo->reload();

      std::string json_response ="{\"filename\":\"" + activeSessions.at(sessionIndex)->getDisplayedFileName()+ "\"}";
      return fromString(json_response, response);
    }


    else
    {
      return fromString("{\"error\":\"Error : this session doesn't exist\"}", response);
    }
  }

}
