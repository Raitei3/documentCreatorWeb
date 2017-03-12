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
    Painter(cv::Mat background, std::vector<cv::Rect> blocks, int characterHeight);
    ~Painter();
    cv::Mat painting();
  void extractFont(std::string fontPath);

  private:
    //QPainter _painter;
  cv::Mat _background;
    int * extractImage(char * str, int size);
    //unsigned long * extractImage(QString str,int size);
    cv::Mat extractImage(QString str, int width, int height);

    std::multimap<char,cv::Mat> _font;
  std::vector<cv::Rect> _blocks;
  int _characterHeight;
  std::string _text = "Loremipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.";
};


#endif //PAINTER_HPP
