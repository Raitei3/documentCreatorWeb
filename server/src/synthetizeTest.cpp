
bool BinarizationTest::getPage(HttpRequest* request, HttpResponse *response)
{
  (void) request;
  (void) response;
  cv::Mat input=cv::imread("data/test.png");
  cv::Mat output;
  Binarization::binarize(input,output);
  cv::imwrite("data/result.png",output);
  return true;
}

bool BackgroundReconstructionTest::getPage(HttpRequest* request, HttpResponse *response)
{
  (void) request;
  (void) response;

  cv::Mat input=cv::imread("data/test.png");
  cv::Mat output;
  Binarization::binarize(input,output);

  QImage binarized = Convertor::getQImage(output);
  QImage origin = Convertor::getQImage(input);

  BackgroundReconstruction back;
  back.setOriginalImage(origin);
  back.setBinarizedImage(binarized);
  back.process();
  QImage backgroundResult = back.getResultImage();
  backgroundResult.save("data/backgroundResult.png");
  return true;
}

bool StructureDetectionTest::getPage(HttpRequest* request, HttpResponse *response)
{
  (void) request;
  (void) response;
  cv::Mat origin = cv::imread("data/test.png");
  cv::Mat binarize;
  Binarization::binarize(origin,binarize);

  int characterHeight = structureDetection::getCharacterHeight(binarize);
  cv::Mat distanceMap = structureDetection::getDistanceMap( origin,binarize);
  std::vector<cv::Rect> block = structureDetection::getBlocks(distanceMap,characterHeight);

  return true;
}

bool FontExtractionTest::getPage(HttpRequest* request, HttpResponse *response)
{
  (void) request;
  (void) response;
  cv::Mat originMat = cv::imread("data/test.png");
  QImage originQImage= Convertor::getQImage(originMat);


  cv::Mat binarizedMat(originMat.rows,originMat.cols, CV_8U);
  Binarization::binarize(originMat,binarizedMat);
  QImage binarizedQImage = Convertor::getQImage(binarizedMat);

  OCR ocr;
  ocr.setParameters("/usr/share/tesseract-ocr/","fra");
  ocr.init(originQImage,binarizedQImage);

  ocr.saveFont("data/test.of");
  return true;
}
