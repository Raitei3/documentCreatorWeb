#ifndef DEGRADATION_HPP
#define DEGRADATION_HPP

#include "util.hpp"

class grayScaleCharsDegradation: public MyDynamicPage
{
  bool getPage(HttpRequest* request, HttpResponse *response);
};

class ShadowBindingDegradation: public MyDynamicPage
{
  bool getPage(HttpRequest* request, HttpResponse *response);
};

class PhantomCharacterDegradation: public MyDynamicPage
{
  bool getPage(HttpRequest* request, HttpResponse *response);
};

class BleedThroughDegradation: public MyDynamicPage
{
  bool getPage(HttpRequest* request, HttpResponse *response);
};

class BlurFilterDegradation: public MyDynamicPage
{
  bool getPage(HttpRequest* request, HttpResponse *response);
};


#endif //DEGRADATION_HPP
