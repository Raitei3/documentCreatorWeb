#include "LoadLetter.hpp"

#include <QFile>
#include <QXmlStreamReader>
#include <iostream>

#include "convertor.h"

using namespace std;

map<string,vector<fontLetter> >  LoadLetter::fromFile(const string &path)
{
  map<string,vector<fontLetter>> fontMap;
  QFile font(QString::fromStdString(path));
  const bool ok = font.open( QFile::ReadOnly );

  if (! ok) {
    std::cerr<<"Warning: unable to open font file: \n";
  }


  QXmlStreamReader reader(&font);
  int width=0;
  int height=0;
  QString s;
  string s2;
  int baseline = 0;
  fontLetter f;

  while(!reader.atEnd())
  {
    QXmlStreamReader::TokenType token = reader.readNext();
    if (token == QXmlStreamReader::StartElement) {
      if(reader.name()=="letter")
      {
        s = reader.attributes().value("char").toString();
        //strcpy(s2, s.toStdString());
        s2 = s.toStdString();
      }

      if (reader.name() == "width") {
        reader.readNext();
        width = reader.text().toString().toInt();
      }
      if (reader.name() == "height") {
        reader.readNext();
        height = reader.text().toString().toInt();
      }
      if (reader.name()=="baseLine") {
        reader.readNext();
        baseline = reader.text().toString().toInt();
      }

      if (reader.name() == "data") {
        reader.readNext();

        QString data = reader.text().toString();

        cv::Mat mat = extractImage(data,width,height);
        f.mask = mat;
        f.baseline = baseline;
        if (fontMap.find(s2) == fontMap.end()) {
          vector<fontLetter>* v = new vector<fontLetter>;
          fontMap.insert(pair<string,vector<fontLetter>>(s2,*v));
          fontMap[s2].push_back(f);
        }
        else{
          fontMap[s2].push_back(f);
        }
      }
    }
  }

  if(reader.hasError())
    cerr<<"Error at line "<<reader.lineNumber()<<" : "<<reader.errorString().toStdString()<<endl;
  return fontMap;
}

map<string,vector<fontLetter> > LoadLetter::fromVector(const vector<fontLetter> &vec)
{
  map<string,vector<fontLetter>> font;
  string s;

  for(auto it=vec.begin();it != vec.end();it++){
    s=it->label;
    font[s].push_back(*it);
    
    cvtColor(font[s].back().mask, font[s].back().mask, CV_GRAY2BGR);
  }
  return font;
}

cv::Mat LoadLetter::extractImage(QString str, int width, int heigth){
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
