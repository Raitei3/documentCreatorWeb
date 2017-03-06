#ifndef GESTIONSESSION_HPP
#define GESTIONSESSION_HPP

class StartSession: public DynamicPage
{
  bool GetPage(HttpRequest* request, HttpResponse *response);
};

class StopSession: public DynamicPage
{
  bool GetPage(HttpRequest* request, HttpResponse *response);
};

class UploadImage: public DynamicPage
{
  bool GetPage(HttpRequest* request, HttpResponse *response);
};

#endif // GESTIONSESSION_HPP
