#ifndef HOLEDEGRADATION_HPP
#define HOLEDEGRADATION_HPP

#include <QColor>
#include <QImage>
#include <opencv2/core/core.hpp>

class Hole
{
  
public :
  
  Hole(int posX, int posY, int size, QColor color, QImage pattern) :
    _posX(posX), _posY(posY), _width(pattern.width()+ size), _height(pattern.height()+size), _size(size), _color(color), _pattern(pattern)
  { }

  int getX() const
  {
    return _posX;
  }

  int getY() const
  {
    return _posY;
  }

  int getWidth() const
  {
    return _width;
  }

  int getHeight() const
  {
    return _height;
  }

  int getSize() const
  {
    return _size;
  }
  
  const QColor &getColor() const
  {
    return _color;
  }

  const QImage &getPattern() const
  {
    return _pattern;
  }
  
private :
  int _posX;
  int _posY;
  int _width;
  int _height;
  int _size;
  QColor _color;
  QImage _pattern;

};

enum class Border {TOP=0, RIGHT, BOTTOM, LEFT};
enum class Corner {TOPLEFT=0, TOPRIGHT, BOTTOMRIGHT, BOTTOMLEFT};
enum class HoleType {CENTER=0, BORDER, CORNER, NUM_HOLE_TYPES};

//side is Border or Corner 

//hole patterns for corner or border must be oriented for top left corner or top border. They will be rotated if necessary.


extern cv::Mat holeDegradation(const cv::Mat &matOriginal, const cv::Mat &matPattern, int xOrigin, int yOrigin, int size, HoleType type, int side, QColor color, const cv::Mat &pageBelow=cv::Mat(), int width=0, float intensity=1000);


extern QImage holeDegradation(const QImage &imgOriginal, const QImage &pattern, int xOrigin, int yOrigin, int size, HoleType type, int side, QColor color, const QImage &pageBelow=QImage(), int width=0, float intensity=1000);


#endif //HOLEDEGRADATION_HPP
