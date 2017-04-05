#include "manualFontExtractor.hpp"


    
bool getBoundingBox::getPage(HttpRequest* request, HttpResponse *response)
{
  std::string token;
  request->getParameter("token", token);
  int sessionIndex = getActiveSessionFromToken(stoi(token));
  if(sessionIndex != -1)
  {
    Image* img = activeSessions.at(sessionIndex)->getImage();
    std::string json = "{\"boundingbox\":{" + img->jsonBoundingRect() + "},\"baseline\":{" + img->jsonBaseline() + "}}";
    return fromString(json, response);
  } else {
    return fromString("{\"error\" : You don't have a valid token, retry please\"}", response);
  }
}


bool getInfoOnCC::getPage(HttpRequest* request, HttpResponse *response)
{
  std::string token;
  std::string ccId;
  std::string lineId;
  request->getParameter("token", token);
  request->getParameter("idCC", ccId);
  request->getParameter("idLine", lineId);
  int sessionIndex = getActiveSessionFromToken(stoi(token));
  if(sessionIndex != -1)
  {
    if(activeSessions.at(sessionIndex)->getImage()->isValidIdCC(stoi(lineId), stoi(ccId)) != -1)
    {
      int sessionIndex = getActiveSessionFromToken(stoi(token));
      cv::Rect bb = activeSessions.at(sessionIndex)->getImage()->getBoundingBoxAtIndex(stoi(ccId), stoi(lineId));
      int charactereId = activeSessions.at(sessionIndex)->getFont()->indexOfCharacterForCC(stoi(ccId), stoi(lineId));

      std::string letter = "";
      int baseline;
      if(charactereId != -1)
      {
        letter = activeSessions.at(sessionIndex)->getFont()->characterAtIndex(charactereId)->getLabel();
        int percent = activeSessions.at(sessionIndex)->getFont()->characterAtIndex(charactereId)->getBaseline();
        baseline = round((float)bb.y + ((float)bb.height * (float)percent / 100));
      } else {
        baseline = activeSessions.at(sessionIndex)->getImage()->getBaselineAtIndex(stoi(ccId), stoi(lineId));
      }
      std::string json = "{";
      json += ("\"id\":" + ccId + ",");
      json += ("\"idLine\":" + lineId + ",");
      json += ("\"baseline\":" + std::to_string(baseline) + ",");
      json += ("\"left\":" + std::to_string(bb.x) + ",");
      json += ("\"right\":" + std::to_string(bb.x + bb.width) + ",");
      json += ("\"up\":" + std::to_string(bb.y) + ",");
      json += ("\"down\":" + std::to_string(bb.y + bb.height) + ",");
      json += ("\"letter\":\"" + letter + "\"");
      json += "}";
      return fromString(json, response);
    } else {
      return fromString("{\"error\" : Your Line and/or id isn't valid, retry please\"}", response);
    }
  } else {
    return fromString("{\"error\" : You don't have a valid token, retry please\"}", response);
  }

}

  
bool updateInfoOnCC::getPage(HttpRequest* request, HttpResponse *response)
{
  std::string listCCId;
  std::string token;
  std::string left;
  std::string right;
  std::string up;
  std::string down;
  std::string letter;
  std::string activeId;
  std::string activeLine;
  std::string baseline;
  request->getParameter("token", token);
  request->getParameter("left", left);
  request->getParameter("right", right);
  request->getParameter("up", up);
  request->getParameter("down", down);
  request->getParameter("letter", letter);
  request->getParameter("id", listCCId);
  request->getParameter("activeid", activeId);
  request->getParameter("activeline", activeLine);
  request->getParameter("baseline", baseline);

  int sessionIndex = getActiveSessionFromToken(stoi(token));
  if(sessionIndex != -1)
  {
    int sessionIndex = getActiveSessionFromToken(stoi(token));
    auto j = json::parse(listCCId);
    // For each component connexe
    for (json::iterator it = j.begin(); it != j.end(); ++it)
    {
      int idCC = it->find("idCC")->get<int>();
      int idLine = it->find("idLine")->get<int>();
      int oldBaseline = 0;
      if(idCC == stoi(activeId) && idLine == stoi(activeLine))
      {
        activeSessions.at(sessionIndex)->getImage()->setBoundingBoxAtIndex(idCC, idLine, stoi(up), stoi(down), stoi(left), stoi(right));
        oldBaseline = activeSessions.at(sessionIndex)->getImage()->getBaselineAtIndex(idCC, idLine);
        activeSessions.at(sessionIndex)->getImage()->setBaselineAtIndex(idCC, idLine, stoi(baseline));
      }
      int indexCharacterForCC = activeSessions.at(sessionIndex)->getFont()->indexOfCharacterForCC(idCC, idLine);
      if(indexCharacterForCC != -1)
      {
        activeSessions.at(sessionIndex)->getFont()->characterAtIndex(indexCharacterForCC)->removeComponent(idCC, idLine);
        if(activeSessions.at(sessionIndex)->getFont()->characterAtIndex(indexCharacterForCC)->countComponent() <= 0)
          activeSessions.at(sessionIndex)->getFont()->removeCharacter(indexCharacterForCC);
      }
      if(letter != "")
      {
        int indexCharacter = activeSessions.at(sessionIndex)->getFont()->indexOfCharacter(letter);
        if(indexCharacter == -1)
        {
          activeSessions.at(sessionIndex)->getFont()->addCharacter(Character(letter));
          indexCharacter = activeSessions.at(sessionIndex)->getFont()->indexOfCharacter(letter);
          activeSessions.at(sessionIndex)->getFont()->characterAtIndex(indexCharacter)->setBaseline(round(((stof(baseline)-stof(up))/(stof(down) - stof(up))) * 100));
        }

        activeSessions.at(sessionIndex)->getFont()->characterAtIndex(indexCharacter)->addComponent(idCC, idLine);

        if(stoi(baseline) != oldBaseline)
        {
          activeSessions.at(sessionIndex)->getFont()->characterAtIndex(indexCharacter)->setBaseline(round(((stof(baseline)-stof(up))/(stof(down) - stof(up))) * 100));
        }
      }

    }
  } else {
    return fromString("{\"error\" : You don't have a valid token, retry please\"}", response);
  }
  return fromString("ok", response);
}



bool merge::getPage(HttpRequest* request, HttpResponse *response)
{

  std::string listCCId;
  std::string token;
  std::string activeId;
  std::string activeLine;
  request->getParameter("token", token);
  request->getParameter("id", listCCId);
  request->getParameter("activeid", activeId);
  request->getParameter("activeline", activeLine);

  int sessionIndex = getActiveSessionFromToken(stoi(token));
  if(sessionIndex != -1)
  {
    int sessionIndex = getActiveSessionFromToken(stoi(token));
    auto j = json::parse(listCCId);
    for (json::iterator it = j.begin(); it != j.end(); ++it)
    {
      int idCC = it->find("idCC")->get<int>();
      int idLine = it->find("idLine")->get<int>();
      if(!(idCC == stoi(activeId) && idLine == stoi(activeLine)))
      {
        cv::Rect bb = activeSessions.at(sessionIndex)->getImage()->getBoundingBoxAtIndex(idCC, idLine);
        cv::Rect bbActive = activeSessions.at(sessionIndex)->getImage()->getBoundingBoxAtIndex(stoi(activeId), stoi(activeLine));
        activeSessions.at(sessionIndex)->getImage()->setBoundingBoxAtIndex(stoi(activeId), stoi(activeLine), std::min(bb.y, bbActive.y), std::max(bb.y + bb.height, bbActive.y + bbActive.height), std::min(bb.x, bbActive.x), std::max(bb.x + bb.width, bbActive.x + bbActive.width));
        //activeSessions.at(sessionIndex)->getImage()->removeConnectedComponentAt(idCC, idLine);
        int indexCharacterForCC = activeSessions.at(sessionIndex)->getFont()->indexOfCharacterForCC(idCC, idLine);
        if(indexCharacterForCC != -1)
        {
          activeSessions.at(sessionIndex)->getFont()->characterAtIndex(indexCharacterForCC)->removeComponent(idCC, idLine);
          if(activeSessions.at(sessionIndex)->getFont()->characterAtIndex(indexCharacterForCC)->countComponent() <= 0)
            activeSessions.at(sessionIndex)->getFont()->removeCharacter(indexCharacterForCC);
        }
      }
    }
    cv::Rect bbActive = activeSessions.at(sessionIndex)->getImage()->getBoundingBoxAtIndex(stoi(activeId), stoi(activeLine));
    std::string json = "{";
    json += ("\"id\":" + activeId + ",");
    json += ("\"idLine\":" + activeLine + ",");
    json += ("\"left\":" + std::to_string(bbActive.x) + ",");
    json += ("\"right\":" + std::to_string(bbActive.x + bbActive.width) + ",");
    json += ("\"up\":" + std::to_string(bbActive.y) + ",");
    json += ("\"down\":" + std::to_string(bbActive.y + bbActive.height));
    json += "}";
    return fromString(json, response);
  } else {
    return fromString("{\"error\" : You don't have a valid token, retry please\"}", response);
  }
  return fromString("ok", response);
}


  
bool extractFont::getPage(HttpRequest* request, HttpResponse *response)
{
  std::string token;
  std::string fontname;
  request->getParameter("token", token);
  request->getParameter("fontname", fontname);

  int sessionIndex = getActiveSessionFromToken(stoi(token));
  if(sessionIndex != -1)
  {
    return fromString(extractFontInOf(sessionIndex, fontname), response);
  } else {
    return fromString("{\"error\" : You don't have a valid token, retry please\"}", response);
  }
}




  
bool updateBaseline::getPage(HttpRequest* request, HttpResponse *response)
{
  std::string token;
  request->getParameter("token", token);
  int sessionIndex = getActiveSessionFromToken(stoi(token));
  if(sessionIndex != -1)
  {
    int sessionIndex = getActiveSessionFromToken(stoi(token));
    std::string idLine;
    request->getParameter("idLine", idLine);
    if(activeSessions.at(sessionIndex)->getImage()->isValidIdLine(stoi(idLine)) != - 1)
    {
      std::string value;
      request->getParameter("value", value);
      activeSessions.at(sessionIndex)->getImage()->setBaselineForLine(stoi(idLine), stoi(value));
      return fromString("ok", response);
    } else {
      return fromString("{\"error\" : You don't have a valid Line, retry please\"}", response);
    }
  } else {
    return fromString("{\"error\" : You don't have a valid token, retry please\"}", response);
  }
}
