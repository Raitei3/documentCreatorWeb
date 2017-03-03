#include "Painter.hpp"

#include <convertor.h>
#include <iostream>
#include <QDebug>
#include <QXmlStreamReader>
#include <QFile>
#include <opencv2/imgproc/imgproc.hpp>
#include <map>
#include <utility>


using namespace std;

Painter::Painter(QImage background, std::vector<cv::Rect> blocks, int characterHeight)
{
  _background=Convertor::getCvMat(background);
  _blocks=blocks;
  _characterHeight=55;//characterHeight;
}

Painter::~Painter()
{
}

QImage Painter::painting()
{
  
  for (auto block=_blocks.begin(); block!=_blocks.end(); block++) {
    //pour deboguer
    //cv::rectangle(_background,*block,0,2);
      
    int line=block->y+_characterHeight;
    int ofset=block->x;
    auto it=_text.begin();
    while(it!=_text.end() && line<block->height+block->y){
      
      
      char c=*it;
      auto fontIt=_font.find(c);
      if(fontIt!=_font.end()){
	cv::Mat pict=fontIt->second;
	int hpict=pict.size().height;
	int wpict=pict.size().width;
	if(c!=' ')//pour éviter un carré gris
	{
          cv::Mat part=_background(cv::Rect(ofset, line-hpict ,wpict, hpict));
	
          part=min(part,pict);//à améliorer

	
	}
	ofset+=wpict;
	if(ofset>block->x+block->width){
	  line+=_characterHeight;
	  ofset=block->x;
	}
      }
      it++;
    }
  }
  return Convertor::getQImage(_background);
}

void Painter::extractFont(string fontPath){

  std::multimap<char,cv::Mat> fontMap;
  QFile font(QString::fromStdString(fontPath));
  const bool ok = font.open( QFile::ReadOnly );

  if (! ok) {
    std::cerr<<"Warning: unable to open font file: \n";
  }

  QXmlStreamReader reader(&font);
  int width=0;
  int height=0;
  QString s;
  char c[5];

  while(!reader.atEnd())
  {
    QXmlStreamReader::TokenType token = reader.readNext();
    if (token == QXmlStreamReader::StartElement) {
      if(reader.name()=="letter")
      {
        s = reader.attributes().value("char").toString();
        strcpy(c, s.toStdString().c_str());
      }

      if (reader.name() == "width") {
        reader.readNext();
        width = reader.text().toString().toInt();
      }
      if (reader.name() == "height") {
        reader.readNext();
        height = reader.text().toString().toInt();
      }

      if (reader.name() == "data") {
        reader.readNext();

        QString data = reader.text().toString();

        cv::Mat mat = extractImage(data,width,height);
        fontMap.insert(multimap<char,cv::Mat>::value_type(c[0],mat));
      }
    }
  }
  if(reader.hasError())
    cerr<<"Error at line "<<reader.lineNumber()<<" : "<<reader.errorString().toStdString()<<endl;
  _font=fontMap;
}



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
