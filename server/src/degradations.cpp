#include "degradation.hpp"
#include "GrayscaleCharsDegradationModel.hpp"
#include "ShadowBinding.hpp"
#include "PhantomCharacter.hpp"
#include "BleedThrough.hpp"
#include "BlurFilter.hpp"

bool grayScaleCharsDegradation::getPage(HttpRequest* request, HttpResponse *response)
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


bool ShadowBindingDegradation::getPage(HttpRequest* request, HttpResponse *response)
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


bool PhantomCharacterDegradation::getPage(HttpRequest* request, HttpResponse *response)
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

bool BleedThroughDegradation::getPage(HttpRequest* request, HttpResponse *response)
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

#if 1
    QImage img_recto((UPLOAD_DIR + activeSessions.at(sessionIndex)->getDisplayedFileName()).c_str());
    QString versoFilename((UPLOAD_DIR + imgVersoParam).c_str());
    QImage img_verso(versoFilename);

    //If image was not loaded correctly
    if (img_verso.width() == 0 || img_verso.height() == 0){
      return fromString("{\"error\":\"Error : The image could not be loaded.\"}", response);
    }

    img_verso = img_verso.mirrored(true, false);

    const QImage img_bleed_through = bleedThrough(img_recto, img_verso, nbIterations);

    //B??? Is it necessary to save image to disk ?  We just wand to transform img_bleed_through in a cv::Mat ???
    QString filename((UPLOAD_DIR + gen_random(".png")).c_str());
    img_bleed_through.save(filename);
    Image img(filename.toStdString());
    activeSessions.at(sessionIndex)->getImage()->setMat(img.getMat());


#else
    //B:TODO: Currently Bleed_Through is coded only with Qt classes (QImage /QThread/...)


    const std::string rectoFilename = UPLOAD_DIR + activeSessions.at(sessionIndex)->getDisplayedFileName();
    cv::Mat img_recto = cv::imread(rectoFilename);
    const std::string versoFilename = UPLOAD_DIR + imgVersoParam;
    cv::Mat img_verso = cv::imread(versoFilename);

    //If image was not loaded correctly
    if (img_verso.rows == 0 || img_verso.cols == 0){
      return fromString("{\"error\":\"Error : The image could not be loaded.\"}", response);
    }

    img_verso = mirror(img_verso);//TODO

    const cv::Mat img_bleed_through = bleedThrough(img_recto, img_verso, nbIterations);

    activeSessions.at(sessionIndex)->getImage()->setMat(img_bleed_through);

#endif //0

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

bool BlurFilterDegradation::getPage(HttpRequest* request, HttpResponse *response)
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
    cv::Mat img_blur = cv::imread(BLUR_IMG_DIR + intensityParam);
    const float dstRadius = getRadiusFourier(img_blur);
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
    const cv::Mat blurredMat = blurFilter(matOut, method, intensity);
    activeSessions.at(sessionIndex)->getImage()->setMat(blurredMat);
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
