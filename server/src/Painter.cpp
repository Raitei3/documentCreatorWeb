#include "Painter.hpp"

#include <convertor.h>
#include <iostream>
#include <QGuiApplication>
#include <QCoreApplication>



Painter::Painter(QImage background, std::vector<cv::Rect> blocks)
{
  _background=background;
  for (size_t i = 0; i < blocks.size(); i++)
  {
    _blocks.append(Convertor::getQRect(blocks.at(i)));
  }
  std::cerr << _blocks[0].topLeft().x()<< std::endl;
}

Painter::~Painter()
{
}

QImage Painter::painting()
{
  int x = 1;
  QGuiApplication app(x,NULL);
  //QGuiApplication app2(x,NULL);

  QPainter _painter(&_background);
  //_painter.begin(&_background);
  std::cerr << _painter.isActive() << std::endl;
  _painter.setPen(Qt::black);
  _painter.setFont(QFont("Arial", 16));
  for (int i = 0; i < _blocks.size(); i++) {
    std::cerr << "tamere"<< std::endl;
    _painter.drawRect(_blocks[i]);
    _painter.drawText(_blocks[i],Qt::TextWordWrap | Qt::TextJustificationForced,_text);
    //_painter.drawEllipse ( _blocks[i] );

    std::cerr << "tamere2"<< std::endl;
  }
  //_painter.end();
  return _background;
}
