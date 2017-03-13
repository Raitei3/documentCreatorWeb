#ifndef SYNTHETIZEIMAGEE_HPP
#define SYNTHETIZEIMAGEE_HPP

#include "util.hpp"
#include "OCR.hpp"

class SynthetizeImage: public MyDynamicPage
{
public:
  bool getPage(HttpRequest* request, HttpResponse *response);
private:
  void binarization();
  void extractBackground();
  void extractFont();
  void extractBlock();
  void createDocument();


  cv::Mat image;
  cv::Mat binarizedImage;
  cv::Mat background;
  OCR ocr;
  std::vector<cv::Rect> blocksImage;
  int characterHeight;
  cv::Mat result;
};

#endif //SYNTHETIZEIMAGEE_HPP
