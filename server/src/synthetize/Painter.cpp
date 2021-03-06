#include "Painter.hpp"

#include <convertor.h>
#include <iostream>
#include <QXmlStreamReader>
#include <cv.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <utility>

#include "LoadLetter.hpp"
#include "util.hpp"

using namespace std;

Painter::Painter(cv::Mat background, vector<cv::Rect> blocks)
{
  _background = background;
  _blocks = blocks;
  widthDoc = _background.cols;
  heightDoc = _background.rows;
}

Painter::~Painter()
{
}

/* Pour chaque paragraphe, écrit le plus de texte possible, renvoie l'image de résultat */
cv::Mat Painter::painting()
{
  cv::Mat background = _background.clone();
  _lineSpacing = computeSpaceLine(_font);
  xml.init(widthDoc,heightDoc,fontName,backgroundName);

  for (auto block=_blocks.begin(); block!=_blocks.end(); block++) {
    if(block->width < _background.cols * 0.05 ){
      continue;
    }
    xml.openBlock(block->x, block->y, block->width, block->height);
    int line = block->y;
    int ofset = block->x;
    auto it = _text.begin();
    
    while(it!=_text.end() && line <block->height+block->y){
      char c=*it;
      auto fontIt = _font.find(string(&c,1));
      
      if(fontIt!=_font.end()){
        int numLetter = rand() % fontIt->second.size();
        cv::Mat pict = fontIt->second[numLetter].mask;
        int baseline = fontIt->second[numLetter].baseline;
        int hpict = pict.size().height;
        int wpict = pict.size().width;
        
        try{
          if(line > 0 && ofset > 0 && ofset + wpict < background.cols &&
             line-baseline*hpict/100 + _lineSpacing + hpict < _background.rows &&
             c!=' ')//pour éviter un carré gris
          {
            /* On récupère la sous image de l'arrière plan qui est située à l'endroit où l'on va
             * écrire le nouveau caractère, et qui fait la même taille que ce dernier.
             * Comme OpenCV n'effectue qu'une copie en surface, si on modifie la sous-
             * image, l'image d'origine sera modifiée en conséquence.
             */
            cv::Mat part=background(cv::Rect(ofset, line-baseline*hpict/100 + _lineSpacing,wpict, hpict));
            /* Pour chaque pixel de coordonnées (i,j), on prend celui étant le plus sombre entre l'arrière plan et
             * la police
             */
            part=min(part,pict);
          }
        }catch(cv::Exception){
          cerr << "erreur : caractère <" << c << ">, ligne =" << line << " et ofset= " << ofset << ".\n"; 
        }
        xml.addLetter(string(&c,1), numLetter, ofset, line-hpict,pict.size().width, pict.size().height);
	ofset += wpict*fontIt->second[numLetter].rightLine/100;
      }
      if(ofset>block->x+block->width || c=='\n'){
        ofset = block->x;
        line += _lineSpacing;
        if(line+_lineSpacing > block->height+block->y)
          break;	
      }
      it++;
    }
    xml.closeBlock();
  }
  xml.close();
  xml.write("data/document.xml");
  return background;
}

void Painter::extractFont(string fontPath)
{
    _font = LoadLetter::fromFile(fontPath);
}

int Painter::computeSpaceLine(map<string,vector<fontLetter> > font)
{
  std::vector<int> aboveBaseline;
  std::vector<int> underBaseline;
  
  for (auto fontIt=font.begin(); fontIt!=font.end(); ++fontIt)
  {
    for (auto it = fontIt->second.begin(); it != fontIt->second.end(); ++it)
    {
      double h = it->mask.size().height;
      int baseline = it->baseline;
      if(baseline < 0 || baseline > 200){
        continue;
      }
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
  
  for(int i =0;i<5; i++){
    aboveMax += aboveBaseline[i];
    underMax += underBaseline[i];
  }
  return (aboveMax+underMax)/5;
}


void Painter::extractFont(vector<fontLetter> fl, cv::Mat background)
{
  _font = LoadLetter::fromVector(fl,background);
}

void Painter::setText(string s)
{
  _text = s;
}

string Painter::saveXML(int token)
{
  xml.write(UPLOAD_DIR + to_string(token) + ".xml");
  return UPLOAD_DIR + to_string(token) + ".xml";
}
