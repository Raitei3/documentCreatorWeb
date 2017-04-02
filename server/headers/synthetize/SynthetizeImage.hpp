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
  cv::Mat binarization(cv::Mat originalImage);
  cv::Mat extractBackground(cv::Mat originalImage, cv::Mat binarizedImage);
  std::vector<fontLetter> extractFont(cv::Mat originalImage, cv::Mat binarizedImage,
                                      std::string language = "eng",
                                      std::string tesseractDir = "/usr/share/tesseract-ocr/");
  std::vector<cv::Rect> extractBlock(cv::Mat originalImage, cv::Mat binarizedImage);
  cv::Mat createDocument(cv::Mat background, std::vector<cv::Rect> blocks,
                         std::vector<fontLetter> font, cv::Mat originalImage);
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
