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

#include <iostream>

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
#include "../headers/BackgroundReconstruction.hpp"
#include "../headers/binarization.hpp"
#include "../headers/convertor.h"
#include "../headers/StructureDetection.hpp"
#include "../headers/Painter.hpp"
#include "../headers/OCR.hpp"
#include "Config.hpp"




class MyDynamicRepository : public DynamicRepository
{
  StartSession startSession;
  StopSession stopSession;
  UploadImage uploadImage;

  getBoundingBox getbounding;
  getInfoOnCC getInfoOnCC;
  updateInfoOnCC updateInfoOnCC;
  merge merge;
  extractFont extractFont;
  updateBaseline updateBaseline;

  grayScaleCharsDegradation grayScaleCharsDegradation;
  ShadowBindingDegradation shadowBindingDegradation;
  PhantomCharacterDegradation phantomCharacterDegradation;
  BleedThroughtDegradation bleedThroughtDegradation;
  BlurPhilterDegradation blurPhilterDegradation;

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
	(void) response;
      response->forwardTo("index.php");
      return true;
    }

  } controller;


  BinarizationTest binarizationTest;
  BackgroundReconstructionTest backgroundReconstructionTest;
  StructureDetectionTest structureDetectionTest;
  FontExtractionTest fontExtractionTest;

  SynthetizeImage synthetizeImage;





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

    add("testBinarization.txt", &BinarizationTest);
    add("backgroundReconstruction.txt", &backgroundReconstructionTest);
    add("structureDetectionTest.txt", &structureDetectionTest);
    add("fontExtractionTest.txt", &fontExtractionTest);
    add("synthetizeImage.txt",&synthetizeImage);
  }
};

/***********************************************************************/

int main(int /*argc*/, char** /*argv*/ )
{
  srand(time(NULL));

  signal( SIGTERM, exitFunction );
  signal( SIGINT, exitFunction );

  NVJ_LOG->addLogOutput(new LogStdOutput);
  Config conf;
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
