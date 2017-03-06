#ifndef SYNTETHIZETEST_HPP
#define SYNTETHIZETEST_HPP


class BinarizationTest: public MyDynamicPage
{
  bool getPage(HttpRequest* request, HttpResponse *response);
};

class BackgroundReconstructionTest: public MyDynamicPage
{
  bool getPage(HttpRequest* request, HttpResponse *response);
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
