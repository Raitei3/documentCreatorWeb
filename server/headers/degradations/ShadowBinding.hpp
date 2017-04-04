#ifndef SHADOWBINDING_HPP
#define SHADOWBINDING_HPP

#include <QImage>
#include <opencv2/core/core.hpp>


/**
   Implements "non-linear illumination model" from :
   "Global and Local Document Degradation Models"
   Tapas Kanungo, Robert M. Haralick and Thsin Phillips
   ICDAR 1993
*/

enum class ShadowBorder {TOP=0, RIGHT, BOTTOM, LEFT};

extern QImage shadowBinding(const QImage &imgOriginal, ShadowBorder border, int distance, float intensity, float angle);

extern void shadowBinding(cv::Mat &matOut, ShadowBorder border, int distance, float intensity, float angle);


#endif //SHADOWBINDING_HPP
