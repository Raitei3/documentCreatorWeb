#include "Painter.hpp"

#include <convertor.h>
#include <iostream>
#include <QDebug>
#include <QXmlStreamReader>
#include <QChar>
#include <cv.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <utility>

#include "LoadLetter.hpp"

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
  computeSpaceLine();
  xml.init(widthDoc,heightDoc,fontName,backgroundName);

  for (auto block=_blocks.begin(); block!=_blocks.end(); block++) {
    xml.openBlock(block->x,block->y,block->width,block->height);
    //pour deboguer
    //cv::rectangle(_background,*block,0,2);
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
        xml.addLetter(string(&c,1),numLetter,ofset,line-hpict,pict.size().width,pict.size().height);
	ofset+=wpict*fontIt->second[numLetter].rightLine/100;
	if(ofset>block->x+block->width){
	  line+=_lineSpacing;
	  ofset=block->x;
	}
      }

      it++;
    }
    xml.closeBlock();

  }
  xml.close();
  xml.write("data/document.xml");
  return _background;
}






void Painter::extractFont(string fontPath){
    _font=LoadLetter::fromFile(fontPath);
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


void Painter::extractFont(vector<fontLetter> fl){
    _font=LoadLetter::fromVector(fl);
}

void Painter::setText(string s){
  _text = s;
}
