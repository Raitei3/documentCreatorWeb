#include <unordered_set>

#include "util.hpp"

/* Const integer for random number of name file generation */
const int rng = 10;

WebServer *webServer = NULL;

LocalRepository *myUploadRepo = NULL;

std::vector<Session*> activeSessions;

const char *CLIENT_DIR = "../client/";
const char *UPLOAD_DIR = "../client/data/";
const char *BLUR_IMG_DIR = "data/image/blurImages/blurExamples/";
//const char *FONT_DIR = "data/font/";
//const char *BACKGROUND_DIR = "data/background/";

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

//inline
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

inline
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
inline
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
