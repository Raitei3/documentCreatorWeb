#include "GetSynthetizeImage.hpp"

#include <string>

#include "SynthetizeImage.hpp"

bool GetSynthetizeImage::getPage(HttpRequest* request, HttpResponse *response)
{
  const char* url = request->getUrl();
  std::string tokenParam;
  request->getParameter("token", tokenParam);
  int token = stoi(tokenParam);
  int sessionIndex = getActiveSessionFromToken(token);

  if(sessionIndex != -1){
    if (strcmp(url, "composeImage.txt") == 0){
      cv::Mat image = activeSessions.at(sessionIndex)->getImage()->getMat();
      std::string fontPath;
      std::string backgroundPath;
      std::string text;
      
      request->getParameter("font", fontPath);
      request->getParameter("background", backgroundPath);
      request->getParameter("text", text);

      fontPath = "data/font/" + fontPath;
      backgroundPath = "data/background/" + backgroundPath;

      SynthetizeImage synthetizeImage;
      cv::Mat result = synthetizeImage.composeImage(fontPath, backgroundPath, text, token);

      activeSessions.at(sessionIndex)->getImage()->setMat(result);
      activeSessions.at(sessionIndex)->saveDisplayedImage(UPLOAD_DIR);
      myUploadRepo->reload();

      std::string json_response = "{\"filename\":\"" + activeSessions.at(sessionIndex)->getDisplayedFileName() + "\"}";
      return fromString(json_response, response);
    }

    else{
      cv::Mat image = activeSessions.at(sessionIndex)->getImage()->getMat();

      SynthetizeImage synthetizeImage;
      cv::Mat result = synthetizeImage.SynthetizeAuto(image, token);

      activeSessions.at(sessionIndex)->getImage()->setMat(result);
      activeSessions.at(sessionIndex)->saveDisplayedImage(UPLOAD_DIR);

      std::string fontPath = synthetizeImage.saveFont(token);
      std::string backgroundPath = synthetizeImage.saveBackground(token);
      
      myUploadRepo->reload();

      std::string json_response = "{\"filename\":\"" + activeSessions.at(sessionIndex)->getDisplayedFileName() + "\"";
      if(!fontPath.empty()){
        json_response += ",\"fontPath\":\"" + fontPath +"\"";
      }
      if(!backgroundPath.empty()){
        json_response += ",\"backgroundPath\":\"" + backgroundPath + "\"";
      }
      json_response += "}";
      return fromString(json_response, response);
    }
  }
  else{
    return fromString("{\"error\":\"Error : this session doesn't exist\"}", response);
  }
}
