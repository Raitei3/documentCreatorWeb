#include "gestionSession.hpp"
#include "util.hpp"

bool StartSession::getPage(HttpRequest* request, HttpResponse *response)
{
  if (!request->isMultipartContent())
    return false;

  // retrieval of the image
  MPFD::Parser *parser = request->getMPFDparser();
  std::map<std::string,MPFD::Field *> fields=parser->GetFieldsMap();
  std::map<std::string,MPFD::Field *>::iterator it;
  for (it=fields.begin(); it!=fields.end(); ++it)
  {
    if(isFormatSupported(fields[it->first]->GetFileName()))
    {
      if (fields[it->first]->GetType()==MPFD::Field::TextType)
        return false;
      else
      {
        std::string newFileName = gen_random(fields[it->first]->GetFileName().substr(fields[it->first]->GetFileName().find(".")));
        NVJ_LOG->append(NVJ_INFO, "Got file field: [" + it->first + "] Filename:[" + newFileName + "] TempFilename:[" + fields[it->first]->GetTempFileName() + "]\n");


        std::ifstream src(fields[it->first]->GetTempFileName().c_str(), std::ios::binary);
        std::string dstFilename = std::string(UPLOAD_DIR) + newFileName;
        std::ofstream dst(dstFilename.c_str(), std::ios::binary);
        if (!src || !dst)
          NVJ_LOG->append(NVJ_ERROR, "Copy error: check read/write permissions");
        else
          dst << src.rdbuf();
        src.close();
        dst.close();
        myUploadRepo->reload();
        std::string json_Session = InitiateSession(newFileName, request);
        NVJ_LOG->append(NVJ_ERROR, "Start Session : " + json_Session);
        return fromString(json_Session, response);
      }
    } else {
      return fromString("{\"error\":\"This format of image isn't correct\"}", response);
    }

  }
  return true;
}


bool StopSession::getPage(HttpRequest* request, HttpResponse *response)
{
  std::string token;
  request->getParameter("token", token);
  int sessionIndex = getActiveSessionFromToken(stoi(token));
  if (sessionIndex == -1)
  {
    return fromString("{\"error\" : You don't have a valid token, retry please\"}", response);
  }

  std::string originalFileName = activeSessions.at(sessionIndex)->getOriginalFileName();
  if (std::remove((UPLOAD_DIR + originalFileName).c_str()) != 0)
  {
    NVJ_LOG->append(NVJ_ERROR, "Error Deleted - Original Image");
    return fromString("{\"error\":\"An error append when deleting the image\"}", response);
  }

  if(activeSessions.at(sessionIndex)->getOriginalFileName() != activeSessions.at(sessionIndex)->getDisplayedFileName())
  {
    std::vector<std::string> splitStr;
    split(splitStr, activeSessions.at(sessionIndex)->getDisplayedFileName(), '_');
    int numImage = std::stoi(splitStr[1]);

    std::string fileDisplayedPath;
    for (int i = 0; i <= numImage; i++){
      fileDisplayedPath = UPLOAD_DIR + splitStr[0] + "_" + std::to_string(i) + "_" + splitStr[2];

      if(std::remove(fileDisplayedPath.c_str()) != 0)
      {
        NVJ_LOG->append(NVJ_ERROR, "Error Deleted - Degraded Images");
        return fromString("{\"error\":\"An error append when deleting the image\"}", response);
      }
    }
  }
  delete activeSessions.at(sessionIndex);
  activeSessions.erase(activeSessions.begin() + sessionIndex);
  NVJ_LOG->append(NVJ_ERROR, "Deleted Session");
  return fromString("{\"success\":\"Goodbye\"}", response);
}


bool UploadImage::getPage(HttpRequest* request, HttpResponse *response)
{
  if (!request->isMultipartContent())
    return false;

  // retrieval of the image
  MPFD::Parser *parser = request->getMPFDparser();
  std::map<std::string,MPFD::Field *> fields=parser->GetFieldsMap();
  std::map<std::string,MPFD::Field *>::iterator it;
  for (it=fields.begin(); it!=fields.end(); ++it)
  {
    if(! isFormatSupported(fields[it->first]->GetFileName()))
    {
      return fromString("{\"error\":\"This format of image isn't correct\"}", response);
    }
    if (fields[it->first]->GetType()==MPFD::Field::TextType)
    {
      return false;
    }

    std::string newFileName = gen_random(fields[it->first]->GetFileName().substr(fields[it->first]->GetFileName().find(".")));
    NVJ_LOG->append(NVJ_INFO, "Got Image field: [" + it->first + "] Filename:[" + newFileName + "] TempFilename:[" + fields[it->first]->GetTempFileName() + "]\n");

    std::ifstream src(fields[it->first]->GetTempFileName().c_str(), std::ios::binary);
    std::string dstFilename = std::string(UPLOAD_DIR) + newFileName;
    std::ofstream dst(dstFilename.c_str(), std::ios::binary);
    if (!src || !dst){
      NVJ_LOG->append(NVJ_ERROR, "Copy error: check read/write permissions");
    } else {
      dst << src.rdbuf();
    }
    src.close();
    dst.close();
    myUploadRepo->reload();

    std::string json_Session = "{\"filename\":\"" + newFileName + "\"}";
    NVJ_LOG->append(NVJ_ERROR, "Upload Image : " + json_Session);
    return fromString(json_Session, response);
  }
  return true;
}
