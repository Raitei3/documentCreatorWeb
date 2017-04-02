#ifndef SYNTHETIZEIMAGEE_HPP
#define SYNTHETIZEIMAGEE_HPP

#include "util.hpp"
#include "OCR.hpp"

class SynthetizeImage
{
public:
  cv::Mat composeImage(std::string fontPath, std::string backgroundPath, std::string text);
  cv::Mat SynthetizeAuto(cv::Mat img);

private:
  void binarization();
  void extractBackground();
  void extractFont();
  void extractBlock();
  void createDocument();
  cv::Rect createStandardBlock(cv::Mat background);

  cv::Mat image;
  cv::Mat binarizedImage;
  cv::Mat background;
  OCR ocr;
  std::vector<cv::Rect> blocksImage;
  int characterHeight = 100;
  cv::Mat result;
  std::vector<fontLetter> font;
};

#endif //SYNTHETIZEIMAGEE_HPP
