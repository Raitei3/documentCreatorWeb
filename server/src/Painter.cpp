#include "Painter.hpp"

#include <convertor.h>
#include <iostream>
#include <QDebug>
#include <QXmlStreamReader>
#include <QChar>
#include <cv.h>
#include <opencv2/imgproc/imgproc.hpp>

#include <utility>


using namespace std;

Painter::Painter(cv::Mat background, std::vector<cv::Rect> blocks, int characterHeight)
{
  _background=background;
  _blocks=blocks;
  _characterHeight=characterHeight;
  //cv::imwrite("data/debug.png",background);
  widthDoc = _background.cols;
  heightDoc = _background.rows;


}

Painter::~Painter()
{
}

cv::Mat Painter::painting()
{
  //QFile file("data/output.xml");
  computeSpaceLine();
  QFile file(QString::fromStdString("data/output.xml"));
  const bool ok = file.open( QFile::WriteOnly );
  if (! ok) {
    std::cerr<<"Warning: unable to open font file: \n";
  }
  xml.setDevice(&file);
  xml.setAutoFormatting(true);
  initXML();

  for (auto block=_blocks.begin(); block!=_blocks.end(); block++) {
    xmlBlocks(block->x,block->y,block->width,block->height);
    //pour deboguer
    cv::rectangle(_background,*block,0,2);
    int line=block->y;
    int ofset=block->x;
    auto it=_text.begin();
    while(it!=_text.end() && line <block->height+block->y){


      char c=*it;
      auto fontIt=_font.find(string(&c,1));
      if(fontIt!=_font.end()){

        int numLetter = rand() % fontIt->second.size();

        cv::Mat pict=fontIt->second[numLetter].mask;
        int baseline=fontIt->second[numLetter].baseline;
        int hpict=pict.size().height;
        int wpict=pict.size().width;
        try{
	if(c!=' ')//pour éviter un carré gris
	{
          cv::Mat part=_background(cv::Rect(ofset, line-baseline*hpict/100 + _lineSpacing,wpict, hpict));

          part=min(part,pict);//à améliorer


	}
}catch(cv::Exception){}
	ofset+=wpict;
	if(ofset>block->x+block->width){
	  line+=_lineSpacing;
	  ofset=block->x;
	}
      QString display = QChar(c);
      addLetterToXML(display,1,ofset,line-hpict,pict.size().width,pict.size().height);
      }

      it++;
    }
    endXML();

  }
  endXML();
  xml.writeEndDocument();
  return _background;
}






void Painter::extractFont(string fontPath){

  std::map<string,std::vector<fontLetter>> fontMap;
  QFile font(QString::fromStdString(fontPath));
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
          std::vector<fontLetter>* v = new std::vector<fontLetter>;
          fontMap.insert(std::pair<string,vector<fontLetter>>(s2,*v));
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
  _font=fontMap;
}


void Painter::computeSpaceLine(){
  std::vector<int> aboveBaseline;
  std::vector<int> underBaseline;
  for (auto fontIt=_font.begin(); fontIt!=_font.end(); ++fontIt)
  {
    for ( auto it = fontIt->second.begin(); it != fontIt->second.end(); ++it)
    {
      double h = it->mask.size().height;
      int baseline = it->baseline;
      int above = (h/100)*baseline;
      int under = h-above;

      aboveBaseline.push_back(above);
      underBaseline.push_back(under);
    }
  }
  std::sort(aboveBaseline.begin(),aboveBaseline.end(), std::greater<int>());
  std::sort(underBaseline.begin(),underBaseline.end(), std::greater<int>());
  int aboveMax=0;
  int underMax=0;
  for(int i =0;i<5;i++){
    aboveMax += aboveBaseline[i];
    underMax += underBaseline[i];
  }
  _lineSpacing = (aboveMax+underMax)/5;
  cout << "linsespacing = " << _lineSpacing << endl;
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

void Painter::extractFont(vector<fontLetter> fl){
  map<string,vector<fontLetter>> font;
  string s;

  vector<fontLetter>::iterator it = fl.begin();
  for(it;it != fl.end();it++){
    s=it->label;

    if (font.find(s) == font.end()) {
      std::vector<fontLetter>* v = new std::vector<fontLetter>;
      font.insert(std::pair<string,vector<fontLetter>>(s,*v));
      font[s].push_back(*it);
    }
    else{
      font[s].push_back(*it);
    }
    cvtColor(font[s].back().mask, font[s].back().mask, CV_GRAY2BGR);
  }
  _font=font;
}

void Painter::setText(string s){
  _text = s;
}



void Painter::initXML(){
    xml.writeStartElement("document");
    xml.writeAttribute("width",QString::number(widthDoc));
    xml.writeAttribute("height",QString::number(heightDoc));
    xml.writeStartElement("styles");
    xml.writeStartElement("style");
    xml.writeAttribute("name",fontName);
    xml.writeStartElement("style");
    xml.writeTextElement("font", fontName);
    xml.writeEndElement();
    xml.writeEndElement();
    xml.writeEndElement();

    xml.writeStartElement("content");
    xml.writeStartElement("page");
    xml.writeAttribute("backgroundFileName",backgroundName);

}


void Painter::xmlBlocks(int x, int y, int width, int height){
  xml.writeStartElement("textBlock");
  xml.writeAttribute("x",QString::number(x));
  xml.writeAttribute("y",QString::number(y));
  xml.writeAttribute("width",QString::number(width));
  xml.writeAttribute("height",QString::number(height));
  xml.writeAttribute("marginTop",QString::number(0));
  xml.writeAttribute("marginBottom",QString::number(0));
  xml.writeAttribute("marginLeft",QString::number(10));
  xml.writeAttribute("marginRight",QString::number(0));
  xml.writeStartElement("paragraph");
  xml.writeAttribute("lineSpacing",QString::number(116));
  xml.writeAttribute("tabulationSize",QString::number(0));
  xml.writeStartElement("string");
  xml.writeAttribute("style","vesale");
}

void Painter::addLetterToXML(QString display,int id,int x, int y, int width,int height){
  xml.writeStartElement("char");
  xml.writeAttribute("display", display);
  xml.writeAttribute("id", QString::number(id));
  xml.writeAttribute("x", QString::number(x));
  xml.writeAttribute("y", QString::number(y));
  xml.writeAttribute("width", QString::number(width));
  xml.writeAttribute("height", QString::number(height));
  xml.writeEndElement();
}

void Painter::endXML() {
  xml.writeEndElement();
  xml.writeEndElement();
  xml.writeEndElement();
}
