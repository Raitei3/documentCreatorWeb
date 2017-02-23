#include "Painter.hpp"

#include <convertor.h>
#include <iostream>
//#include <QGuiApplication>
//#include <QCoreApplication>
//#include <QXml>
//#include <QDomDocument>
#include <QXmlStreamReader>
#include <QFile>
#include <opencv2/imgproc/imgproc.hpp>


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

void Painter::extractFont(){

  QFile font("data/test.of");
  const bool ok = font.open( QFile::ReadOnly );

  if (! ok) {
    std::cerr<<"Warning: unable to open font file: \n";
    return;
  }

  QXmlStreamReader reader(&font);
  int width=0;
  int height=0;
  QString s;
  const char* c;
  //const char* str;

  //while (!reader.atEnd())
  int i =0;
  while(i<200)
  {
    QXmlStreamReader::TokenType token = reader.readNext();
    if (token == QXmlStreamReader::StartElement) {
      if(reader.name()=="letter")
      {
        //std::cerr<<"name ="<<reader.name().toString().toStdString()<<"\n";
        s = reader.attributes().value("char").toString();
        //std::cerr<<"char ="<<s.toStdString()<<".\n";
        c= s.toStdString().c_str();

      }
      s = reader.attributes().value("char").toString();
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

      if (reader.name() == "data" && *c!=' ') {
        reader.readNext();

        printf("%d,%d ,%s\n",width,height,c );
        QString data = reader.text().toString();
      //  str = (const char *)malloc(sizeof(char)*100000);
      //  str = data.toStdString().c_str();
        //printf("%s\n",str );
	//unsigned  long * array = extractImage(data,width*height);
	QImage im=extractImage(data,width,height);
	//cv::Mat mat(height,width,CV_32FC4, &array);
	cv::Mat mat=Convertor::getCvMat(im);
      cv::imwrite("data/charactere.png",mat);
        //std::cerr<<"char ="<<data.toStdString()<<".\n";
        return;
      }
  }
    i++;
    //return;
  }
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

QImage Painter::extractImage(QString str, int width, int heigth){
    QImage ret=QImage(width,heigth,QImage::Format_ARGB32);
    QStringList list=str.split(',');
    auto it=list.begin();
    for(int j=0; j<heigth; j++){
	QRgb* d=(QRgb*)ret.scanLine(j);
	for(int i=0; i<width; i++,it++){
	    d[i]=it->toUInt();
	}
    }
    return ret;
}
