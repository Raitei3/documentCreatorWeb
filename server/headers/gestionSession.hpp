#ifndef GESTIONSESSION_HPP
#define GESTIONSESSION_HPP

#include <libnavajo/libnavajo.hh>

class StartSession: public DynamicPage
{
public:
  bool getPage(HttpRequest* request, HttpResponse *response);
};

class StopSession: public DynamicPage
{
  bool getPage(HttpRequest* request, HttpResponse *response);
};

class UploadImage: public DynamicPage
{
  bool getPage(HttpRequest* request, HttpResponse *response);
};

#endif // GESTIONSESSION_HPP
