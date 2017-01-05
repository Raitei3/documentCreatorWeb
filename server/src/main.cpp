/*
 * inspired from example from Thierry DESCOMBES, Creator of Libnavajo Server
 */

#include "libnavajo/libnavajo.hh"
#include "libnavajo/LogStdOutput.hh"

#include <ctime>
#include <signal.h> 
#include <cstring> 
#include <unordered_set>

//manipulation des répertoires
#include <dirent.h>
#include <sys/stat.h>
#include <cstdio> //remove

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

#include <QFileInfo>

using json = nlohmann::json;


static const char *CLIENT_DIR = "../client/";
static const char *UPLOAD_DIR = "../client/data/";
static const char *BLUR_IMG_DIR = "data/image/blurImages/blurExamples/";
//static const char *FONT_DIR = "data/font/";
//static const char *BACKGROUND_DIR = "data/background/";



/* Const integer for random number of name file generation */
const int rng = 10;

WebServer *webServer = NULL;

LocalRepository *myUploadRepo = NULL;

std::vector<Session*> activeSessions;



/* split the string @a s */
int split(std::vector<std::string>& v, const std::string &s, char separateur)
{
  v.clear();

  std::string ss = s;
  
  std::string::size_type stTemp = ss.find(separateur);
	
  while(stTemp != std::string::npos)
  {
    v.push_back(ss.substr(0, stTemp));
    ss = ss.substr(stTemp + 1);
    stTemp = ss.find(separateur);
  }
	
  v.push_back(ss);

  return v.size();
}


/*
 * \brief Function use when the service is closed and killed the active session
 *
 * \param dummy : the dummy as integer 
 */

void exitFunction( int /*dummy*/ )
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
bool isFormatSupported( const std::string &filename)
{
  std::string extension = filename.substr(filename.find(".") + 1);
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
std::string gen_random(const std::string &extension)
{
  static const char letter[] =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";
  
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
std::string InitiateSession(const std::string &filename, HttpRequest* /*request*/)
{
  int cptExample = 0;
  Session* mySession = new Session((UPLOAD_DIR) + filename);
  
  cptExample = rand();
  mySession->setToken(cptExample);
  mySession->setOriginalFileName(filename);   
  activeSessions.push_back(mySession);
  mySession->getImage()->ComputeMask();
  return "{\"filename\":\"" + filename + "\",\"token\":" + std::to_string(mySession->getToken()) + "}";
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

    
  class StartSession: public DynamicPage
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
  } startSession;

  
  class stopSession: public MyDynamicPage
  {
    bool getPage(HttpRequest* request, HttpResponse *response)
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

  } stopSession;

  
  class UploadImage: public DynamicPage
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
  } uploadImage;


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


  class extractFont: public MyDynamicPage
  {
    bool getPage(HttpRequest* request, HttpResponse *response)
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
        NVJ_LOG->append(NVJ_ERROR, "Degradation - GrayScale Character : level = " + levelParam + ";");
        
        std::string json_response ="{\"filename\":\"" + activeSessions.at(sessionIndex)->getDisplayedFileName()+ "\"}";
        return fromString(json_response, response); 
      }
      else
      {
        return fromString("{\"error\":\"Error : this session doesn't exist\"}", response);
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
        NVJ_LOG->append(NVJ_ERROR, "Degradation - Shadow Binding : border = " + borderParam + "; width = " + widthParam + "; intensity = " + intensityParam + "; angle = " + angleParam + ";");
        
        std::string json_response ="{\"filename\":\"" + activeSessions.at(sessionIndex)->getDisplayedFileName()+ "\"}";
        return fromString(json_response, response); 
      }
      else
      {
        return fromString("{\"error\":\"Error : this session doesn't exist\"}", response);
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
        NVJ_LOG->append(NVJ_ERROR, "Degradation - Phantom Character : frequency = " + frequencyParam + ";");
        
        std::string json_response ="{\"filename\":\"" + activeSessions.at(sessionIndex)->getDisplayedFileName()+ "\"}";
        return fromString(json_response, response); 
      }
      else
      {
        return fromString("{\"error\":\"Error : this session doesn't exist\"}", response);
      }
    }
  } phantomCharacterDegradation;
  
  
  class BleedThroughDegradation: public MyDynamicPage
  {
    bool getPage(HttpRequest* request, HttpResponse *response)
    {

      std::string tokenParam;
      std::string nbIterationsParam;
      std::string imgVersoParam;
      
      request->getParameter("token", tokenParam);
      request->getParameter("nbIterations", nbIterationsParam);
      request->getParameter("imgVerso", imgVersoParam);

      int nbIterations = stoi(nbIterationsParam);

      int token = stoi(tokenParam);
      int sessionIndex = getActiveSessionFromToken(token);
      if(sessionIndex != -1)
      {
        QImage img_recto((UPLOAD_DIR + activeSessions.at(sessionIndex)->getDisplayedFileName()).c_str());
        QString versoFilename((UPLOAD_DIR + imgVersoParam).c_str());
        QImage img_verso(versoFilename);

        // Si l'image n'a pas bien chargé
        if (img_verso.width() == 0 || img_verso.height() == 0){
          return fromString("{\"error\":\"Error : The image could not be loaded.\"}", response);
        }

        img_verso = img_verso.mirrored(true, false);
        
        QImage img_bleed_through = bleedThrough(img_recto, img_verso, nbIterations);
        
        QString filename((UPLOAD_DIR + gen_random(".png")).c_str());
        img_bleed_through.save(filename);
        Image img(filename.toStdString()); 
        
        activeSessions.at(sessionIndex)->getImage()->setMat(img.getMat());
        activeSessions.at(sessionIndex)->saveDisplayedImage(UPLOAD_DIR);
        myUploadRepo->reload();
        NVJ_LOG->append(NVJ_ERROR, "Degradation - Bleed Through : number iterations = " + nbIterationsParam + "; image verso = " + UPLOAD_DIR + imgVersoParam + ";");
        
        std::string json_response = "{\"filename\":\"" + activeSessions.at(sessionIndex)->getDisplayedFileName()+ "\"}";
        return fromString(json_response, response); 
      }
      else
      {
        return fromString("{\"error\":\"Error : this session doesn't exist\"}", response);
      }

    }
  } bleedThroughDegradation;

  
  class BlurFilterDegradation: public MyDynamicPage
  {
    bool getPage(HttpRequest* request, HttpResponse *response)
    {      
      std::string tokenParam;
      std::string methodParam;
      std::string typeIntensityParam;
      std::string intensityParam;
      
      request->getParameter("token", tokenParam);
      request->getParameter("method", methodParam);
      request->getParameter("typeIntensity", typeIntensityParam);
      request->getParameter("intensity", intensityParam);

      int intensity;
      if (typeIntensityParam == "value"){
        intensity = stoi(intensityParam);
      } else { // typeIntensityParam == "image"
        QImage img_blur((BLUR_IMG_DIR + intensityParam).c_str());
        float dstRadius = getRadiusFourier(img_blur);
        intensity = searchFitFourier(img_blur, dstRadius);
      }

      Method method;
      if (methodParam == "gaussian") {
        method = Method::GAUSSIAN;
      } else if (methodParam == "median") {
        method = Method::MEDIAN;
      } else { // methodParam == "normal"
        method = Method::NORMAL;
      }

      int token = stoi(tokenParam);
      int sessionIndex = getActiveSessionFromToken(token);
      if(sessionIndex != -1)
      {
        cv::Mat matOut = activeSessions.at(sessionIndex)->getImage()->getMat();
        activeSessions.at(sessionIndex)->getImage()->setMat(blurFilter(matOut, method, intensity));
        activeSessions.at(sessionIndex)->saveDisplayedImage(UPLOAD_DIR);
        myUploadRepo->reload();
        NVJ_LOG->append(NVJ_ERROR, "Degradation - Blur Filter : typeIntensity = " + typeIntensityParam + "; image/value = " + (BLUR_IMG_DIR + intensityParam) + "; intensity = " +  std::to_string(intensity) + "; method = " + methodParam + ";");
        
        std::string json_response ="{\"filename\":\"" + activeSessions.at(sessionIndex)->getDisplayedFileName()+ "\"}";
        return fromString(json_response, response); 
      }
      else
      {
        return fromString("{\"error\":\"Error : this session doesn't exist\"}", response);
      }
    }
  } blurFilterDegradation;


  class DownloadCreateDocument: public MyDynamicPage
  {
    bool getPage(HttpRequest* request, HttpResponse *response)
    {
      std::string typeDownload;
      std::string font;
      std::string background;
      std::string text;
 
      request->getParameter("typeDownload", typeDownload);
      request->getParameter("font", font);
      request->getParameter("background", background);
      request->getParameter("text", text);

      
      
      NVJ_LOG->append(NVJ_ERROR, "Create Document - typeDownload = " + typeDownload + "; font = " + font + "; background = " +  background + "; text = \"" + text + "\";");
      return fromString(background, response); 
    }
  } downloadCreateDocument;


  class GetElemsDirectory: public MyDynamicPage
  {
    bool getPage(HttpRequest* request, HttpResponse *response)
    {
      std::string directoryName;
      request->getParameter("directory", directoryName);
      
      const char* directoryPath = ("data/" + directoryName).c_str();
      DIR* directory = opendir(directoryPath);
      struct dirent* file = NULL;
      
      std::string messageConsole = "getElemsDirectory - directory = data/" + directoryName + "/;\n";
;
      std::string listFiles;
      int nbFiles = 0;
      while ((file = readdir(directory)) != NULL){
        if(file->d_type != DT_DIR){
          listFiles.append(file->d_name);
          listFiles.append(";");
          nbFiles++;
          messageConsole.append("  - " + std::string(file->d_name) + "\n");
        }
      }
      
      listFiles.insert(0, ";");
      listFiles.insert(0, std::to_string(nbFiles));

      NVJ_LOG->append(NVJ_ERROR, messageConsole);

      return fromString(listFiles, response); 
    }
  } getElemsDirectory;

  
  class Controller: public MyDynamicPage
  {
    bool getPage(HttpRequest* /*request*/, HttpResponse *response)
    {
      response->forwardTo("index.php");
      return true;
    }

  } controller;

 public:
  MyDynamicRepository() : DynamicRepository()
  {
    add("index.html", &controller);
    add("startSession.txt", &startSession);
    add("stopSession.txt", &stopSession);
    add("uploadImage.txt", &uploadImage);

    add("getBoundingBox.txt", &getBoundingBox);
    add("getInfoOnCC.txt", &getInfoOnCC);
    add("updateInfoOnCC.txt", &updateInfoOnCC);
    add("extractFont.txt", &extractFont);
    add("updateBaseline.txt", &updateBaseline);
    add("merge.txt", &merge);
    
    add("grayScaleCharsDegradation.txt", &grayScaleCharsDegradation);
    add("shadowBinding.txt", &shadowBindingDegradation);
    add("phantomCharacter.txt", &phantomCharacterDegradation);
    add("bleedThrough.txt", &bleedThroughDegradation);
    add("blurFilter.txt", &blurFilterDegradation);

    add("downloadCreateDocument.txt", &downloadCreateDocument);
    add("getElemsDirectory.txt", &getElemsDirectory);
  }
};

/***********************************************************************/

int main(int /*argc*/, char** /*argv*/ )
{
  srand(time(NULL));
  
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


