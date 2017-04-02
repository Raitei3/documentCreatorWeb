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
#include <unistd.h>
#include <sys/wait.h>

#include "Image.hpp"
#include "Font.hpp"
#include "Session.hpp"
#include "json.hpp"
#include "GrayscaleCharsDegradationModel.hpp"
#include "ShadowBinding.hpp"
#include "PhantomCharacter.hpp"
#include "HoleDegradation.hpp"
#include "BlurFilter.hpp"
#include "BleedThrough.hpp"
#include "BackgroundReconstruction.hpp"
#include "binarization.hpp"
#include "convertor.h"
#include "StructureDetection.hpp"
#include "Painter.hpp"
#include "OCR.hpp"
#include "Config.hpp"

#include "GetSynthetizeImage.hpp"
#include "degradation.hpp"
#include "gestionSession.hpp"
#include "manualFontExtractor.hpp"
#include "synthetizeTest.hpp"
#include "util.hpp"
#include "StartSession2.hpp"

class MyDynamicRepository : public DynamicRepository
{
  StartSession2 _startSession;
  StopSession _stopSession;
  UploadImage _uploadImage;

  getBoundingBox _getBoundingBox;
  getInfoOnCC _getInfoOnCC;
  updateInfoOnCC _updateInfoOnCC;
  merge _merge;
  extractFont _extractFont;
  updateBaseline _updateBaseline;

  grayScaleCharsDegradation _grayScaleCharsDegradation;
  ShadowBindingDegradation _shadowBindingDegradation;
  PhantomCharacterDegradation _phantomCharacterDegradation;
  BleedThroughDegradation _bleedThroughDegradation;
  BlurFilterDegradation _blurFilterDegradation;

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
      closedir(directory);

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


  BinarizationTest _binarizationTest;
  BackgroundReconstructionTest _backgroundReconstructionTest;
  StructureDetectionTest _structureDetectionTest;
  FontExtractionTest _fontExtractionTest;

  GetSynthetizeImage _getSynthetizeImage;





 public:
  MyDynamicRepository() : DynamicRepository()
  {
    add("index.html", &controller);
    add("startSession.txt", &_startSession);
    add("stopSession.txt", &_stopSession);
    add("uploadImage.txt", &_uploadImage);

    add("getBoundingBox.txt", &_getBoundingBox);
    add("getInfoOnCC.txt", &_getInfoOnCC);
    add("updateInfoOnCC.txt", &_updateInfoOnCC);
    add("extractFont.txt", &_extractFont);
    add("updateBaseline.txt", &_updateBaseline);
    add("merge.txt", &_merge);

    add("grayScaleCharsDegradation.txt", &_grayScaleCharsDegradation);
    add("shadowBinding.txt", &_shadowBindingDegradation);
    add("phantomCharacter.txt", &_phantomCharacterDegradation);
    add("bleedThrough.txt", &_bleedThroughDegradation);
    add("blurFilter.txt", &_blurFilterDegradation);

    add("downloadCreateDocument.txt", &downloadCreateDocument);
    add("getElemsDirectory.txt", &getElemsDirectory);

    add("testBinarization.txt", &_binarizationTest);
    add("backgroundReconstruction.txt", &_backgroundReconstructionTest);
    add("structureDetectionTest.txt", &_structureDetectionTest);
    add("fontExtractionTest.txt", &_fontExtractionTest);
    add("synthetizeImage.txt",&_getSynthetizeImage);
    add("composeImage.txt",&_getSynthetizeImage);
  }
};

/***********************************************************************/

int main(int /*argc*/, char** /*argv*/ )
{
  srand(time(NULL));

  Config conf;
  bool run=true;
  while(run){
    int pid;
    pid=fork();
    if(pid==0){
      std::cerr<<"Server launched with pid "<<getpid()<<std::endl;
      atexit(exitFunction);
      NVJ_LOG->addLogOutput(new LogStdOutput);
      webServer = new WebServer;
      webServer->setSocketTimeoutInSecond(10);
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
    else{
      int status;
      waitpid(pid,&status,0);
      if((WIFSIGNALED(status) && WTERMSIG(status)==15)||
         WIFEXITED(status)){//tant que le serveur n'as pas terminé normalement ou par le signal 15, on le relance
        run=false;
      }
    }
  }
}
