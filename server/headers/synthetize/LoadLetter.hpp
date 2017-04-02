#ifndef LOAD_LETTER_HPP
#define LOAD_LETTER_HPP

#include <map>
#include <string>
#include <vector>
#include <cv.h>

#include "OCR.hpp"

class LoadLetter
{
public:
    static std::map<std::string,std::vector<fontLetter> > fromFile(const std::string &path);
  static std::map<std::string,std::vector<fontLetter> > fromVector(const std::vector<fontLetter> &vec,
                                                                   const cv::Mat &background);

private:
    static cv::Mat extractImage(QString str, int width, int heigth);
  static cv::Mat  getImageFromMask(const cv::Mat &original, const cv::Mat &mask, int background_value);
};

#endif
