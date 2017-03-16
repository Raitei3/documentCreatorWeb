#ifndef STRUCTUREDETECTION_H
#define STRUCTUREDETECTION_H


#include <opencv2/core/core.hpp>


class structureDetection
{
public:
  static std::vector<cv::Rect> getBlocks(const cv::Mat &img);
  static std::vector<cv::Rect> getBlocks(const cv::Mat &distanceMap, int dilateFactor = 1);
  static std::vector<cv::Rect> getWordBlocks(const cv::Mat &img_bin);
  static cv::Mat getDistanceMap(const cv::Mat &img, const cv::Mat &img_bin);
  static int getCharacterHeight(const cv::Mat &img);
};

#endif // STRUCTUREDETECTION_H
