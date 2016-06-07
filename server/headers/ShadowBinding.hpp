#ifndef SHADOWBINDING_HPP
#define SHADOWBINDING_HPP

#include <opencv2/core/core.hpp>


/**
   Implements "non-linear illumination model" from :
   "Global and Local Document Degradation Models"
   Tapas Kanungo, Robert M. Haralick and Thsin Phillips
   ICDAR 1993
*/

enum class ShadowBorder {TOP=0, RIGHT, BOTTOM, LEFT};

void shadowBinding(cv::Mat &matOut, ShadowBorder border, int distance, float intensity, float angle);

#endif //SHADOWBINDING_HPP
