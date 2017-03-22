#include "StartSession2.hpp"
#include "util.hpp"
bool StartSession2::getPage(HttpRequest* request, HttpResponse *response)
{
    if(request->getRequestType()==POST_METHOD){
        return _startSession.getPage(request,response);
    }
    else{
        std::string json_Session = InitiateSession("../../server/data/background/newBackGround2.png", request);
        NVJ_LOG->append(NVJ_ERROR, "Start Session : " + json_Session);
        return fromString(json_Session, response);
    }
}
