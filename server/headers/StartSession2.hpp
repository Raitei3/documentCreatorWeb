#include "gestionSession.hpp"

class StartSession2: public DynamicPage
{
public:
    bool getPage(HttpRequest* request, HttpResponse *response);

private:
    StartSession _startSession;
};
