#include "libnavajo/libnavajo.hh"
#include "libnavajo/LogStdOutput.hh"

#include <time.h>
#include <signal.h> 
#include <string.h> 
#include <unordered_set>

#include "../headers/Image.hpp"
#include "../headers/Font.hpp"
#include "../headers/Session.hpp"
#include "../headers/json.hpp"
#include "../headers/GrayscaleCharsDegradationModel.hpp"
#include "../headers/ShadowBinding.hpp"
#include "../headers/PhantomCharacter.hpp"
#include "../headers/HoleDegradation.hpp"
#include "../headers/BlurFilter.hpp"
#include "../headers/BleedThrough.hpp"


using json = nlohmann::json;


static const char *CLIENT_DIR = "../client/";
static const char *UPLOAD_DIR = "../client/data/";


/* Const integer for random number of name file generation */
const int rng = 10;

WebServer *webServer = NULL;

LocalRepository *myUploadRepo = NULL;

std::vector<Session*> activeSessions;

/*
 * \brief Function use when the service is closed and killed the active session
 *
 * \param dummy : the dummy as integer 
 */

void exitFunction( int dummy )
{
  for (unsigned i=0; i<activeSessions.size(); i++)
  {
    delete activeSessions.at(i);
  }
  if (webServer != NULL)
  {
    webServer->stopService();
  }
}

/*
 * \brief Verify if the format's image uploaded is supported 
 *
 * \param &filenane : the filename as string 
 *
 * \return a bool
 */
static inline
bool isFormatSupported( const std::string &fileName)
{
  std::string extension = fileName.substr(fileName.find(".") + 1);
  std::transform(extension.begin(), extension.end(), extension.begin(), ::toupper);
  std::unordered_set<std::string> format ={"JPG","JPEG","PNG","TIFF","TIF"};
  std::unordered_set<std::string>::const_iterator got = format.find(extension);
  if (got != format.end()){
    return true;
  }
  return false;
}

/*
 * Auxialiry function to save one instance of character to XML file
 *
 */
static inline
void
saveInstanceToXml(std::ostream &xmlDocument, int id, int width, int height, const std::string &data)
{
  xmlDocument << "<picture id=\"" + std::to_string(id) + "\">"<< "\n";
  xmlDocument << "<imageData>"<< "\n";
  xmlDocument << "<width>" + std::to_string((int)width)+"</width>\n";
  xmlDocument << "<height>" + std::to_string((int)height)+"</height>\n";
  xmlDocument << "<format>5</format>"<< "\n";
  xmlDocument << "<degradationlevel>0</degradationlevel>"<< "\n";
  xmlDocument << "<data>" + data + "</data>"<< "\n";
  xmlDocument << "</imageData>"<< "\n";
  xmlDocument << "</picture>"<< "\n";
}

/*
 * Make data for space character (that is, transparent white square) as a string
 */
static inline
std::string
makeSpaceCharacterData(int width, int height)
{
  const unsigned int v = (255<<16)|(255<<8)|(255);
  const std::string pix = std::to_string(v)+",";

  std::string data;
  data.reserve(width*height*pix.size());
  for (int i=0; i<width*height; ++i) {
    data.append(pix);
  }
  return data;
}

/*
 * \brief Extract all informations about the font  
 *
 * \param sessionindex : the index as integer
 * \param fontname : the fontname as string 
 *
 * \return a string
 */
static
std::string extractFontInOf(int sessionIndex, const std::string &fontName)
{
  std::ostringstream xmlDocument;
  xmlDocument << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << "\n" << "<font name=\"" + fontName + "\">" << "\n";
  const Image *image = activeSessions.at(sessionIndex)->getImage();
  const Font *font = activeSessions.at(sessionIndex)->getFont();
  const int nbCharacters = font->countCharacter();
  const std::string SPACE = " ";
  bool hasSpace = false;
  std::vector<int> widths;
  std::vector<int> heights;
  const size_t reserveSize = nbCharacters*4; //arbitrary
  widths.reserve(reserveSize);
  heights.reserve(reserveSize);

  for(int i = 0; i < nbCharacters; ++i)
  {
    const Character* character = font->characterAtIndex(i);
    xmlDocument << "<letter char=\"" +  character->getLabel() + "\">"<< "\n";
    xmlDocument << "<anchor>"<< "\n";
    xmlDocument << "<upLine>0</upLine>"<< "\n";
    xmlDocument << "<baseLine>" << character->getBaseline() << "</baseLine>"<< "\n";
    xmlDocument << "<leftLine>0</leftLine>"<< "\n";
    xmlDocument << "<rightLine>100</rightLine>"<< "\n";
    xmlDocument << "</anchor>" << "\n";

    if (! hasSpace) {
      if (character->getLabel() == SPACE) {
	hasSpace = true;
      }
    }

    const int nbComponents = character->countComponent();
    for(int j = 0; j < nbComponents; ++j)
    {
      std::pair<int, int> ids = character->getIdComponentAtIndex(j);
      int indexLine = ids.first;
      int indexCC = ids.second;

      ConnectedComponent component = image->getConnectedComponnentAt(indexCC, indexLine);
      cv::Rect bb = image->getBoundingBoxAtIndex(indexCC, indexLine);

      saveInstanceToXml(xmlDocument, j, bb.width, bb.height, image->extractDataFromComponent(indexCC, indexLine));

      widths.push_back(bb.width);
      heights.push_back(bb.height);
    }
    xmlDocument << "</letter>" << "\n";

  }

  if (! hasSpace) {
    const size_t numCCs = widths.size();
    if (numCCs > 0) {
      //which size should have the space character ?
      //Here we take the median width & height...
      auto itW = widths.begin()+numCCs/2;
      std::nth_element(widths.begin(), itW, widths.end());
      auto itH = heights.begin()+numCCs/2;
      std::nth_element(heights.begin(), itH, heights.end());
      const int w = *itW;
      const int h = *itH;

      //NVJ_LOG->append(NVJ_INFO, "Add space character in font w="+std::to_string(w)+" h="+std::to_string(h));

      xmlDocument << "<letter char=\"" + SPACE + "\">"<< "\n";
      xmlDocument << "<anchor>"<< "\n";
      xmlDocument << "<upLine>0</upLine>"<< "\n";
      xmlDocument << "<baseLine>100</baseLine>"<< "\n";
      xmlDocument << "<leftLine>0</leftLine>"<< "\n";
      xmlDocument << "<rightLine>100</rightLine>"<< "\n";
      xmlDocument << "</anchor>" << "\n";
      saveInstanceToXml(xmlDocument, 0, w, h, makeSpaceCharacterData(w, h));
      xmlDocument << "</letter>" << "\n";
    }
  }

  xmlDocument << "</font>" << "\n";

  return xmlDocument.str();
} 

/*
 * \brief Generate a random name for image uploaded 
 *
 * \param extension : the extension as string
 *
 * \return a string
 */
static
std::string gen_random(std::string extension) {
  static const char letter[] =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";
  srand(time(NULL));
  
  std::string random;
  for (int i = 0; i < rng; ++i) {
    random += letter[rand() % strlen(letter)];
  }

  return random + extension;
}

/*
 * \brief Creater User session when image was uploading
 *
 * \param filenane : the filename as string 
 * \param *request : the request as HttpRequest
 *
 * \return a string JSON
 */
static
std::string InitiateSession(std::string fileName, HttpRequest *request)
{
  int cptExample = 0;
  Session * mySession = new Session((UPLOAD_DIR)+fileName);
  
  srand(time(NULL));
  cptExample = rand();
  mySession->setToken(cptExample);
  mySession->setOriginalFileName((UPLOAD_DIR)+fileName);   
  activeSessions.push_back(mySession);
  mySession->getImage()->ComputeMask();
  return "{\"fileName\":\""+fileName+"\",\"token\":" + std::to_string(mySession->getToken())+"}";
}

/*
 * \brief Verify if token is valid
 *
 * \param token : the token as integer
 *
 * \return a integer
 */
static
int getActiveSessionFromToken(int token)
{
  for (unsigned i=0; i<activeSessions.size(); i++)
  {
    const Session *s = activeSessions.at(i);
    if(s->getToken() == token)
      return i;
  }
  return -1;
}



class MyDynamicRepository : public DynamicRepository
{
  class MyDynamicPage : public DynamicPage
  {    
  };

  class Uploader: public DynamicPage
  {
    bool getPage(HttpRequest* request, HttpResponse *response)
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
	    std::string dstFilename = std::string(UPLOAD_DIR)+newFileName;
            std::ofstream dst(dstFilename.c_str(), std::ios::binary);
            if (!src || !dst)
              NVJ_LOG->append(NVJ_ERROR, "Copy error: check read/write permissions");
            else
              dst << src.rdbuf();
            src.close();
            dst.close();
            myUploadRepo->reload();
            std::string json_Session = InitiateSession(newFileName, request);
            NVJ_LOG->append(NVJ_ERROR, json_Session);
            return fromString(json_Session, response); 
          }
        } else {
          return fromString("{\"error\":\"This format of image isn't correct\"}", response);
        }

      }
      return true;
    }
  } uploader;

  class getBoundingBox: public MyDynamicPage
  {
    /*
     * \brief Extract all bounding box and baseline
     *
     * \param *response : the response as HttpReponse 
     * \param *request : the request as HttpRequest
     *
     * \return a string in response and a bool
     */
    bool getPage(HttpRequest* request, HttpResponse *response)
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

  } getBoundingBox;

  class getInfoOnCC: public MyDynamicPage
  { 
    /*
     * \brief Get informations about a bounding box
     *
     * \param *response : the response as HttpReponse 
     * \param *request : the request as HttpRequest
     *
     * \return a string in response and a bool
     */
    bool getPage(HttpRequest* request, HttpResponse *response)
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

  } getInfoOnCC;    

  class updateInfoOnCC: public MyDynamicPage
  { 
    /*
     * \brief Update informations about a bounding box
     *
     * \param *response : the response as HttpReponse 
     * \param *request : the request as HttpRequest
     *
     * \return a string in response and a bool
     */
    bool getPage(HttpRequest* request, HttpResponse *response)
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

  } updateInfoOnCC;

  class merge: public MyDynamicPage
  { 
    /*
     * \brief Merge the components
     *
     * \param *response : the response as HttpReponse 
     * \param *request : the request as HttpRequest
     *
     * \return a string in response and a bool
     */
    bool getPage(HttpRequest* request, HttpResponse *response)
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

  } merge;


  class stopSession: public MyDynamicPage
  {
    bool getPage(HttpRequest* request, HttpResponse *response)
    {
      std::string token;
      request->getParameter("token", token);
      int sessionIndex = getActiveSessionFromToken(stoi(token));
      if(sessionIndex != -1)
      {
        std::string filePath = activeSessions.at(sessionIndex)->getOriginalFileName();
        if( remove( filePath.c_str() ) != 0 )
        {
          NVJ_LOG->append(NVJ_ERROR, "Error Deleted");
          return fromString("{\"error\":\"An error append when deleting the image\"}", response);
        } else {
          if(activeSessions.at(sessionIndex)->getOriginalFileName() != activeSessions.at(sessionIndex)->getDisplayedFileName())
          {
            std::string fileDisplayedPath = UPLOAD_DIR + activeSessions.at(sessionIndex)->getDisplayedFileName();
            if( remove( fileDisplayedPath.c_str()) != 0 )
            {
              NVJ_LOG->append(NVJ_ERROR, "Error Deleted");
              return fromString("{\"error\":\"An error append when deleting the image\"}", response);
            }
          }
          delete activeSessions.at(sessionIndex);
          activeSessions.erase(activeSessions.begin() + sessionIndex);
          NVJ_LOG->append(NVJ_ERROR, "Deleted");
          return fromString("{\"success\":\"Goodbye\"}", response);
        }
      } else {
        return fromString("{\"error\" : You don't have a valid token, retry please\"}", response);
      }
    }

  } stopSession;

  class extractFont: public MyDynamicPage
  {
    bool getPage(HttpRequest* request, HttpResponse *response)
    {
      std::string token;
      request->getParameter("token", token);
      int sessionIndex = getActiveSessionFromToken(stoi(token));
      if(sessionIndex != -1)
      { 
        int sessionIndex = getActiveSessionFromToken(stoi(token));
        std::string fontname;
        request->getParameter("fontname", fontname);
        return fromString(extractFontInOf(sessionIndex, fontname), response);
      } else {
        return fromString("{\"error\" : You don't have a valid token, retry please\"}", response);
      }
    }

  } extractFont;


  class updateBaseline: public MyDynamicPage
  {
    /*
     * \brief Update informations about a baseline
     *
     * \param *response : the response as HttpReponse 
     * \param *request : the request as HttpRequest
     *
     * \return a string in response and a bool
     */
    bool getPage(HttpRequest* request, HttpResponse *response)
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

  } updateBaseline;  

  class grayScaleCharsDegradation: public MyDynamicPage
  {
    bool getPage(HttpRequest* request, HttpResponse *response)
    {
      std::string tokenParam;
      std::string levelParam;
      request->getParameter("token", tokenParam);
      request->getParameter("level", levelParam);
      int token = stoi(tokenParam);
      int level = stoi(levelParam);
      int sessionIndex = getActiveSessionFromToken(token);
      if(sessionIndex != -1)
      {
        GrayscaleCharsDegradationModel grayDegradation = GrayscaleCharsDegradationModel(activeSessions.at(sessionIndex)->getImage()->getMat());
        activeSessions.at(sessionIndex)->getImage()->setMat(grayDegradation.degradateByLevel_cv(level));
        activeSessions.at(sessionIndex)->saveDisplayedImage(UPLOAD_DIR);
        myUploadRepo->reload();
        return fromString(activeSessions.at(sessionIndex)->getDisplayedFileName(), response);
      }
      else
      {
        return fromString("Error : this session doesn't exist", response);
      }

    }
  } grayScaleCharsDegradation;


  
  class ShadowBindingDegradation: public MyDynamicPage
  {
    bool getPage(HttpRequest* request, HttpResponse *response)
    {
      std::string tokenParam;
      std::string borderParam;
      std::string widthParam;
      std::string intensityParam;
      std::string angleParam;
      
      request->getParameter("token", tokenParam);
      request->getParameter("border", borderParam);
      request->getParameter("width", widthParam);
      request->getParameter("intensity", intensityParam);
      request->getParameter("angle", angleParam);
      
      int token = stoi(tokenParam);
      
      ShadowBorder border;
      if (borderParam.compare("left") == 0){
        border = ShadowBorder::LEFT;
      } else if (borderParam.compare("right") == 0){
        border = ShadowBorder::RIGHT;
      } else if (borderParam.compare("top") == 0){
        border = ShadowBorder::TOP;
      } else {
        border = ShadowBorder::BOTTOM;
      }
      
      int width = stoi(widthParam);
      float intensity = stof(intensityParam);
      float angle = stof(angleParam);
      
      int sessionIndex = getActiveSessionFromToken(token);
      if(sessionIndex != -1)
      {
        cv::Mat matOut = activeSessions.at(sessionIndex)->getImage()->getMat();
        shadowBinding(matOut, border, width, intensity, angle);
        activeSessions.at(sessionIndex)->getImage()->setMat(matOut);
        activeSessions.at(sessionIndex)->saveDisplayedImage(UPLOAD_DIR);
        myUploadRepo->reload();
        return fromString(activeSessions.at(sessionIndex)->getDisplayedFileName(), response);
      }
      else
      {
        return fromString("Error : this session doesn't exist", response);
      }

    }
  } shadowBindingDegradation;

  
  class PhantomCharacterDegradation: public MyDynamicPage
  {
    bool getPage(HttpRequest* request, HttpResponse *response)
    {
      std::string tokenParam;
      std::string frequencyParam;

      request->getParameter("token", tokenParam);
      request->getParameter("frequency", frequencyParam);
      
      int token = stoi(tokenParam);
      int frequency = stoi(frequencyParam);

      int sessionIndex = getActiveSessionFromToken(token);
      if(sessionIndex != -1)
      {
        cv::Mat matOut = activeSessions.at(sessionIndex)->getImage()->getMat();
        activeSessions.at(sessionIndex)->getImage()->setMat(phantomCharacter(matOut, frequency));
        activeSessions.at(sessionIndex)->saveDisplayedImage(UPLOAD_DIR);
        myUploadRepo->reload();
        return fromString(activeSessions.at(sessionIndex)->getDisplayedFileName(), response);
      }
      else
      {
        return fromString("Error : this session doesn't exist", response);
      }

    }
  } phantomCharacterDegradation;
  

  class UploaderImgRectoBleedThrough: public DynamicPage
  {
    bool getPage(HttpRequest* request, HttpResponse *response)
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
        NVJ_LOG->append(NVJ_INFO, "Got Image Recto field: [" + it->first + "] Filename:[" + newFileName + "] TempFilename:[" + fields[it->first]->GetTempFileName() + "]\n");

        std::ifstream src(fields[it->first]->GetTempFileName().c_str(), std::ios::binary);
        std::string dstFilename = std::string(UPLOAD_DIR)+newFileName;
        std::ofstream dst(dstFilename.c_str(), std::ios::binary);
        if (!src || !dst){
          NVJ_LOG->append(NVJ_ERROR, "Copy error: check read/write permissions");
        } else {
          dst << src.rdbuf();
        }
        src.close();
        dst.close();
        myUploadRepo->reload();
        std::string json_Session = "{\"fileName\":\"" + newFileName + "\"}";
        NVJ_LOG->append(NVJ_ERROR, json_Session);
        return fromString(json_Session, response); 
      }
      return true;
    }
  } uploaderImgRectoBleedThrough;
  
  
  class BleedThroughDegradation: public MyDynamicPage
  {
    bool getPage(HttpRequest* request, HttpResponse *response)
    {      
      std::string tokenParam;
      std::string nbIterationsParam;
      std::string imgRectoParam;
      
      request->getParameter("token", tokenParam);
      request->getParameter("nbIterations", nbIterationsParam);
      request->getParameter("imgRecto", imgRectoParam);

      int nbIterations = stoi(nbIterationsParam);
      
      int token = stoi(tokenParam);
      int sessionIndex = getActiveSessionFromToken(token);
      if(sessionIndex != -1)
      {
        QImage img_verso((UPLOAD_DIR + imgRectoParam).c_str());
        img_verso = img_verso.mirrored(true, false);
        
        QImage img_recto(activeSessions.at(sessionIndex)->getDisplayedFileName().c_str());
        QImage img_bleed_through = bleedThrough(img_recto, img_verso, nbIterations);
        
        QString filename((UPLOAD_DIR + gen_random(".png")).c_str());
        img_bleed_through.save(filename);
        Image img(filename.toStdString()); 
        
        activeSessions.at(sessionIndex)->getImage()->setMat(img.getMat());
        activeSessions.at(sessionIndex)->saveDisplayedImage(UPLOAD_DIR);
        myUploadRepo->reload();
        return fromString(activeSessions.at(sessionIndex)->getDisplayedFileName(), response);
      }
      else
      {
        return fromString("Error : this session doesn't exist", response);
      }

    }
  } bleedThroughDegradation;
  
  class Controller: public MyDynamicPage
  {
    bool getPage(HttpRequest* request, HttpResponse *response)
    {
      response->forwardTo("index.php");
      return true;
    }

  } controller;

 public:
  MyDynamicRepository() : DynamicRepository()
  {
    add("index.html",&controller);
    add("uploader.txt",&uploader);
    add("getBoundingBox.txt",&getBoundingBox);
    add("getInfoOnCC.txt",&getInfoOnCC);
    add("updateInfoOnCC.txt",&updateInfoOnCC);
    add("stopSession.txt",&stopSession);
    add("extractFont.txt",&extractFont);
    add("updateBaseline.txt",&updateBaseline);
    add("merge.txt",&merge);
    add("grayScaleCharsDegradation.txt",&grayScaleCharsDegradation);
    add("shadowBinding.txt",&shadowBindingDegradation);
    add("phantomCharacter.txt",&phantomCharacterDegradation);
    add("bleedThrough.txt",&bleedThroughDegradation);
    add("uploaderImgRectoBleedThrough.txt",&uploaderImgRectoBleedThrough);
  }
};

/***********************************************************************/

/*
 * \author Provided by Thierry DESCOMBES, Creator of Libnavajo Server
 */
int main(int argc, char** argv )
{
  signal( SIGTERM, exitFunction );
  signal( SIGINT, exitFunction );

  NVJ_LOG->addLogOutput(new LogStdOutput);

  webServer = new WebServer;

  //webServer->setUseSSL(true, "../mycert.pem");
  LocalRepository *myLocalRepo = new LocalRepository("", CLIENT_DIR);
  //myLocalRepo.addDirectory("", "../client/"); 
  webServer->addRepository(myLocalRepo);

  MyDynamicRepository myRepo;
  webServer->addRepository(&myRepo);

  myUploadRepo = new LocalRepository("data", UPLOAD_DIR);
  webServer->addRepository(myUploadRepo);


  webServer->startService();

  webServer->wait();

  LogRecorder::freeInstance();

  return 0;
}


