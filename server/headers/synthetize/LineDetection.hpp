#ifndef LINEDETECTIONDIALOG_HPP
#define LINEDETECTIONDIALOG_HPP

/*#include <QImage>
#include <QObject>*/
#include <QDialog>

#include "opencv2/core/core.hpp"


class QLabel;

class LineDetectionDialog : public QObject
{
public:
    explicit LineDetectionDialog(QObject *parent = 0);
    ~LineDetectionDialog();
    void setOriginalImage(const QImage &img);
    QImage getResultImage() { return _houghImg; }

protected:
    void process();
    int getCharacterHeight(cv::Mat img);
    void drawBaseLines(const cv::Mat &img, cv::Mat &dst, const std::vector<cv::Vec4i> &lines);

private:
    QLabel *_thumbnail;
    QLabel *_originalLabel;

    QImage _originalImg;
    QImage _houghImg;

    int _character_height;
};

#endif // LINEDETECTIONDIALOG_HPP
