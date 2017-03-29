#include "SynthetizeImage.hpp"
#include "GetSynthetizeImage.hpp"


bool GetSynthetizeImage::getPage(HttpRequest* request, HttpResponse *response)
{
  const char* url = request->getUrl();

  std::cout << url << std::endl;
    std::string tokenParam;
    request->getParameter("token", tokenParam);
    int token = stoi(tokenParam);
    int sessionIndex = getActiveSessionFromToken(token);
    std::cout << tokenParam << std::endl;
    if(sessionIndex != -1)
    {

      if (!strcmp(url,"composeImage.txt")) {
      cv::Mat image = activeSessions.at(sessionIndex)->getImage()->getMat();
      std::string fontPath;
      std::string backgroundPath;
      std::string text;
      request->getParameter("font",fontPath);
      request->getParameter("background",backgroundPath);
      request->getParameter("text",text);

      fontPath = "data/font/"+fontPath;
      backgroundPath= "data/background/"+backgroundPath;

      SynthetizeImage synthetizeImage;
      cv::Mat result = synthetizeImage.composeImage(fontPath,backgroundPath,text);


      activeSessions.at(sessionIndex)->getImage()->setMat(result);
      activeSessions.at(sessionIndex)->saveDisplayedImage(UPLOAD_DIR);
      myUploadRepo->reload();

      std::string json_response ="{\"filename\":\"" + activeSessions.at(sessionIndex)->getDisplayedFileName()+ "\"}";
      return fromString(json_response, response);
  }

  else{
      cv::Mat image = activeSessions.at(sessionIndex)->getImage()->getMat();

      SynthetizeImage synthetizeImage;
      cv::Mat result = synthetizeImage.SynthetizeAuto(image);

      activeSessions.at(sessionIndex)->getImage()->setMat(result);
      activeSessions.at(sessionIndex)->saveDisplayedImage(UPLOAD_DIR);
      myUploadRepo->reload();

      std::string json_response ="{\"filename\":\"" + activeSessions.at(sessionIndex)->getDisplayedFileName()+ "\"}";
      return fromString(json_response, response);
  }
}
    else
    {
      return fromString("{\"error\":\"Error : this session doesn't exist\"}", response);
    }
}
