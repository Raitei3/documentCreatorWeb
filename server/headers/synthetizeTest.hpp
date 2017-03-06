#ifndef SYNTETHIZETEST_HPP
#define SYNTETHIZETEST_HPP

#include <libnavajo/libnavajo.hh>

#include "util.hpp"

class BinarizationTest: public MyDynamicPage
{
  bool getPage(HttpRequest* request, HttpResponse *response);
};

class BackgroundReconstructionTest: public MyDynamicPage
{
  bool getPage(HttpRequest* request, HttpResponse *response);
public:
  static QImage getBackgroundMain(cv::Mat input);
};

class StructureDetectionTest: public MyDynamicPage
{
  bool getPage(HttpRequest* request, HttpResponse *response);
};

class FontExtractionTest: public MyDynamicPage
{
  bool getPage(HttpRequest* request, HttpResponse *response);
};


#endif //SYNTETHIZETEST_HPP
