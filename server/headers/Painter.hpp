#ifndef PAINTER_HPP
#define PAINTER_HPP

#include <QImage>
#include <QFont>
#include <QRect>
#include <QString>
#include <QPainter>
#include "opencv2/core/core.hpp"

class Painter
{
  public:
    Painter(QImage background, std::vector<cv::Rect> blocks);
    ~Painter();
    //QImage painting();
    std::multimap<char,cv::Mat> extractFont();

  private:
    //QPainter _painter;
    QImage _background;
    int * extractImage(char * str, int size);
    //unsigned long * extractImage(QString str,int size);
    cv::Mat extractImage(QString str, int width, int height);
    QVector<QRect> _blocks;
    QString _text = "Loremipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
};


#endif //PAINTER_HPP
