#ifndef CONVERTOR_H
#define CONVERTOR_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <QImage>

class Convertor
{
public:
    Convertor();
    static cv::Mat getCvMat(const QImage &src);
    static QImage getQImage(const cv::Mat &src);
    static cv::Mat binarizeOTSU(const cv::Mat &src);
};

#endif // CONVERTOR_H