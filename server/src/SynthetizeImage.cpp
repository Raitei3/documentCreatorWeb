#include "SynthetizeImage.hpp"
#include "binarization.hpp"
#include "OCR.hpp"
#include "convertor.h"
#include "StructureDetection.hpp"
#include "Painter.hpp"
#include "synthetizeTest.hpp"

bool SynthetizeImage::getPage(HttpRequest* request, HttpResponse *response)
{
  std::string tokenParam;
  request->getParameter("token", tokenParam);
  int token = stoi(tokenParam);
  int sessionIndex = getActiveSessionFromToken(token);
  if(sessionIndex != -1)
  {
    cv::Mat origin = activeSessions.at(sessionIndex)->getImage()->getMat();
    //cv::Mat origin=cv::imread("data/image/Bmt_res2812_002.png");
    cv::Mat binarize=cv::Mat(origin.rows,origin.cols, CV_8U);
    Binarization::preProcess(origin, binarize, 12);
    Binarization::NiblackSauvolaWolfJolion(binarize, binarize, Binarization::WOLFJOLION, 128, 40, 40, 0.34);
    Binarization::postProcess(binarize, binarize, 0.9, 7);
    

    OCR ocr;
    ocr.setParameters("/usr/share/tesseract-ocr/","eng");
    ocr.init(Convertor::getQImage(origin),Convertor::getQImage(binarize));
    ocr.saveFont("data/test2.of");

    QImage background = BackgroundReconstructionTest::getBackgroundMain(origin);


    int characterHeight = structureDetection::getCharacterHeight(binarize);
    cv::Mat distanceMap = structureDetection::getDistanceMap( origin,binarize);
    std::vector<cv::Rect> blocks = structureDetection::getBlocks(distanceMap,characterHeight);

	  

    Painter painter(background,blocks,characterHeight);
    painter.extractFont("data/test2.of");
    QImage result = painter.painting();
    cv::Mat cvResult = Convertor::getCvMat(result);

    activeSessions.at(sessionIndex)->getImage()->setMat(cvResult);
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
