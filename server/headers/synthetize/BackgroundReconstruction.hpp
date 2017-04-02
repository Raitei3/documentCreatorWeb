#ifndef BACKGROUNDRECONSTRUCTION_HPP
#define BACKGROUNDRECONSTRUCTION_HPP

#include <QDialog>
#include <QObject>

#include <opencv2/core/core.hpp>


class QLabel;

namespace Ui {
class BackgroundReconstruction;
}

class BackgroundReconstruction : public QObject
{
public:
    explicit BackgroundReconstruction(QObject *parent = 0);
    ~BackgroundReconstruction();
    void setOriginalImage(const QImage &img);
    void setBinarizedImage(const QImage &img);
    QImage getResultImage() const {return _background;}

    void process(bool preview=false);

    float getMaxTextArea() const {  return (float)max_text_area*0.01;}
    float getMaxTextWidth() const { return (float)max_text_area*0.005;}
    float getMaxTextHeight() const {return (float)max_text_area*0.005;}

private:
    int max_text_area = 20;

    cv::Mat _originalImg;
    cv::Mat _binarizedImg;

    cv::Mat _smallOriginalImg;
    cv::Mat _smallBinarizedImg;
    QImage _background;
    QImage _structure;
};

#endif // BACKGROUNDRECONSTRUCTIONDIALOG_HPP
