#ifndef MANUALFONTEXTRACTOR_HPP
#define MANUALFONTEXTRACTOR_HPP

#include <libnavajo/libnavajo.hh>

#include "util.hpp"

class getBoundingBox: public MyDynamicPage
{
  /*
   * \brief Extract all bounding box and baseline
   *
   * \param *response : the response as HttpReponse
   * \param *request : the request as HttpRequest
   *
   * \return a string in response and a bool
   */
  bool getPage(HttpRequest* request, HttpResponse *response);
};

class getInfoOnCC: public MyDynamicPage
{
  /*
   * \brief Get informations about a bounding box
   *
   * \param *response : the response as HttpReponse
   * \param *request : the request as HttpRequest
   *
   * \return a string in response and a bool
   */
  bool getPage(HttpRequest* request, HttpResponse *response);
};

class updateInfoOnCC: public MyDynamicPage
{
  /*
   * \brief Update informations about a bounding box
   *
   * \param *response : the response as HttpReponse
   * \param *request : the request as HttpRequest
   *
   * \return a string in response and a bool
   */
  bool getPage(HttpRequest* request, HttpResponse *response);
};


class merge: public MyDynamicPage
{
  /*
   * \brief Merge the components
   *
   * \param *response : the response as HttpReponse
   * \param *request : the request as HttpRequest
   *
   * \return a string in response and a bool
   */
  bool getPage(HttpRequest* request, HttpResponse *response);
};

class extractFont: public MyDynamicPage
{
  bool getPage(HttpRequest* request, HttpResponse *response);
};

class updateBaseline: public MyDynamicPage
{
  /*
   * \brief Update informations about a baseline
   *
   * \param *response : the response as HttpReponse
   * \param *request : the request as HttpRequest
   *
   * \return a string in response and a bool
   */
  bool getPage(HttpRequest* request, HttpResponse *response);
};



#endif //MANUALFONTEXTRACTOR_HPP
