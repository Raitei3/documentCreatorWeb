#ifndef SYNTHETIZEIMAGEE_HPP
#define SYNTHETIZEIMAGEE_HPP

#include "util.hpp"
#include "OCR.hpp"
#include "Painter.hpp"

class SynthetizeImage
{
public:
  cv::Mat composeImage(std::string fontPath, std::string backgroundPath,
                       std::string text, int token);
  cv::Mat SynthetizeAuto(cv::Mat img, int token);
  std::string saveFont(int token);
  std::string saveBackground(int token);

private:
  cv::Mat binarization(cv::Mat originalImage);
  cv::Mat extractBackground(cv::Mat originalImage, cv::Mat binarizedImage);
  std::vector<fontLetter> extractFont(cv::Mat originalImage, cv::Mat binarizedImage,
                                      std::string language = "eng",
                                      std::string tesseractDir = "/usr/share/tesseract-ocr/");
  std::vector<cv::Rect> extractBlock(cv::Mat originalImage, cv::Mat binarizedImage);
  cv::Mat createDocument(cv::Mat background, std::vector<cv::Rect> blocks,
                         std::vector<fontLetter> font, cv::Mat originalImage, int token);
  std::vector<cv::Rect> createStandardBlock(cv::Mat background);

  OCR ocr;
  cv::Mat _background;
};

#endif //SYNTHETIZEIMAGEE_HPP
