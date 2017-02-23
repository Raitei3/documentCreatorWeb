#include "Painter.hpp"

#include <convertor.h>
#include <iostream>
//#include <QGuiApplication>
//#include <QCoreApplication>
//#include <QXml>
//#include <QDomDocument>
#include <QDebug>
#include <QXmlStreamReader>
#include <QFile>
#include <opencv2/imgproc/imgproc.hpp>
#include <map>
#include <utility>


using namespace std;

Painter::Painter(QImage background, std::vector<cv::Rect> blocks)
{
  _background=background;
  for (size_t i = 0; i < blocks.size(); i++)
  {
    _blocks.append(Convertor::getQRect(blocks.at(i)));
  }
}

Painter::~Painter()
{
}

/*QImage Painter::painting()
{
  int x = 1;
  //QGuiApplication app(x,NULL);
  //QGuiApplication app2(x,NULL);

  QPainter _painter(&_background);
  //_painter.begin(&_background);
  _painter.setPen(Qt::black);
  _painter.setFont(QFont("Arial", 16));
  for (int i = 0; i < _blocks.size(); i++) {
    _painter.drawRect(_blocks[i]);
    _painter.drawText(_blocks[i],Qt::TextWordWrap | Qt::TextJustificationForced,_text);
    //_painter.drawEllipse ( _blocks[i] );

  }
  //_painter.end();
  return _background;
}
*/

std::multimap<char,cv::Mat> Painter::extractFont(){

  std::multimap<char,cv::Mat> fontMap;
  QFile font("data/test.of");
  const bool ok = font.open( QFile::ReadOnly );

  if (! ok) {
    std::cerr<<"Warning: unable to open font file: \n";
  }

  QXmlStreamReader reader(&font);
  int width=0;
  int height=0;
  QString s;
  char c[5];

  //while (!reader.atEnd())
  while(!reader.atEnd())
  {
    QXmlStreamReader::TokenType token = reader.readNext();
    if (token == QXmlStreamReader::StartElement) {
      if(reader.name()=="letter")
      {
        //std::cerr<<"name ="<<reader.name().toString().toStdString()<<"\n";
        s = reader.attributes().value("char").toString();
        //std::cerr<<"char ="<<s.toUtf8()<<".\n";
        //QDebug()<< s;
        strcpy(c, s.toStdString().c_str());
        //c2=(char*)c;
      }
      //s = reader.attributes().value("char").toString();
      //std::cerr<<"char ="<<s.toStdString()<<".\n";

      if (reader.name() == "width") {
        reader.readNext();
        width = reader.text().toString().toInt();
        //printf("%d,%d\n",width,height );
      }
      if (reader.name() == "height") {
        reader.readNext();
        height = reader.text().toString().toInt();
      }

      if (reader.name() == "data") {
        reader.readNext();

        //printf("%d,%d ,%s\n",width,height,c );
        QString data = reader.text().toString();

        cv::Mat mat = extractImage(data,width,height);
        //fontMap.insert(std::pair<char,int>(*c,mat));
        //printf("%s\n",c );
        fontMap.insert(multimap<char,cv::Mat>::value_type(c[0],mat));
        //fontMap [*c] = mat;

      }
    }
    /*if (token == QXmlStreamReader::EndElement && reader.name()=="letter"){

    }*/
  }
  return fontMap;
}

int* Painter::extractImage(char * str, int size){
  int* array = (int*)malloc(sizeof(int)*size);
  char * token;
  const char s[2]=",";
  int i=0;
  printf("%s\n",str );
  token = strtok(str,s);
  while (token != NULL) {
    int x = atoi(token);
    array[i]= x;
    token = strtok(NULL,s);
    printf("%d\n",array[i] );
    i++;
  }
  return array;
}

// unsigned long * Painter::extractImage(QString str,int size){
//   unsigned long* array = (unsigned long*)malloc(sizeof(unsigned long)*size);
//   QStringList list = str.split(',');
//   for (int i =0 ; i<size;i++){
//     array[i]=atol(list.at(i).toLocal8Bit().constData());
//     printf("%lu\n",array[i] );
//   }
//   return array;
// }

cv::Mat Painter::extractImage(QString str, int width, int heigth){
  QImage ret=QImage(width,heigth,QImage::Format_ARGB32);
  QStringList list=str.split(',');
  auto it=list.begin();
  for(int j=0; j<heigth; j++){
    QRgb* d=(QRgb*)ret.scanLine(j);
    for(int i=0; i<width; i++,it++){
      d[i]=it->toUInt();
    }
  }
  cv::Mat mat=Convertor::getCvMat(ret);
  return mat;
}
