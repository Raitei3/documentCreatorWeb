#include "Session.hpp"
#include <fstream>


Session::Session(const std::string &f):m_originalFileName(""),m_displayedFileName(""),m_token(0),m_font(new Font()),m_image(new Image(f))
{}
Session::Session(const Session& session):m_originalFileName(""),m_displayedFileName(""),m_token(session.getToken()),m_font(nullptr), m_image(nullptr)
{
}
Session::~Session()
{
  delete m_image;
  m_image = nullptr;
  delete m_font;
  m_font = nullptr;
}

void Session::setOriginalFileName(const std::string &f){
  m_originalFileName = f;
  if(m_displayedFileName == "")
    m_displayedFileName = f;
}

std::string Session::getOriginalFileName() const{
  return m_originalFileName;
}

void Session::setDisplayedFileName(const std::string &f){
  m_displayedFileName = f;
}

std::string Session::getDisplayedFileName() const{
  return m_displayedFileName;
}

Image* Session::getImage(){
  return m_image;
}
Font* Session::getFont(){
  return m_font;
}

void Session::setToken(int t){
  m_token = t;
}

int Session::getToken() const{
  return m_token;
}

void Session::saveDisplayedImage(const std::string &upload_dir){
  static int cpt = -1;
  static int token = m_token;
  if (token != m_token)
  {
    token = m_token;
    cpt = -1;
  }
  
  cpt++;
  
  m_displayedFileName = std::to_string(m_token) + "_" + std::to_string(cpt) + "_Displayed.png";

  cv::imwrite(upload_dir + m_displayedFileName, m_image->getMat());
}

