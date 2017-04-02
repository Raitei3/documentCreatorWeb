#include "StartSession2.hpp"
#include "util.hpp"

using namespace std;

bool StartSession2::getPage(HttpRequest* request, HttpResponse *response)
{
  if(request->getRequestType()==POST_METHOD){
    return _startSession.getPage(request,response);
  }
  else{
    string nameCopy=gen_random(".png");
    ifstream orig("data/small.png");
    ofstream dest(UPLOAD_DIR + nameCopy);
    cerr<<nameCopy<<endl;
    dest<<orig.rdbuf();
    orig.close();
    dest.close();
    string json_Session = InitiateSession(nameCopy, request);
    NVJ_LOG->append(NVJ_ERROR, "Start Session : " + json_Session);
    return fromString(json_Session, response);
  }
}
