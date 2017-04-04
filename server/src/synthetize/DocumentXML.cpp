#include "DocumentXML.hpp"

#include <QFile>
#include <QTextStream>

using namespace std;

//écrit le document xml dans un QString interne à la classe
documentXML::documentXML():
    stream(&xml)
{
  stream.setAutoFormatting(true);
}

//ouvre le document
void documentXML::init(int widthDoc, int heightDoc, string fontName, string backgroundName)
{
  stream.writeStartElement("document");
  stream.writeAttribute("width",QString::number(widthDoc));
  stream.writeAttribute("height",QString::number(heightDoc));
  stream.writeStartElement("styles");
  stream.writeStartElement("style");
  stream.writeAttribute("name",QString::fromStdString(fontName));
  stream.writeStartElement("style");
  stream.writeTextElement("font", QString::fromStdString(fontName));
  stream.writeEndElement();
  stream.writeEndElement();
  stream.writeEndElement();

  stream.writeStartElement("content");
  stream.writeStartElement("page");
  stream.writeAttribute("backgroundFileName",QString::fromStdString(backgroundName));
}

void documentXML::close()
{
  stream.writeEndElement();
  stream.writeEndElement();
  stream.writeEndElement();
}

//commence un paragraphe
void documentXML::openBlock(int x, int y, int width, int height)
{
  stream.writeStartElement("textBlock");
  stream.writeAttribute("x",QString::number(x));
  stream.writeAttribute("y",QString::number(y));
  stream.writeAttribute("width",QString::number(width));
  stream.writeAttribute("height",QString::number(height));
  stream.writeAttribute("marginTop",QString::number(0));
  stream.writeAttribute("marginBottom",QString::number(0));
  stream.writeAttribute("marginLeft",QString::number(10));
  stream.writeAttribute("marginRight",QString::number(0));
  stream.writeStartElement("paragraph");
  stream.writeAttribute("lineSpacing",QString::number(116));
  stream.writeAttribute("tabulationSize",QString::number(0));
  stream.writeStartElement("string");
  stream.writeAttribute("style","vesale");
}

void documentXML::closeBlock()
{
  stream.writeEndElement();
  stream.writeEndElement();
  stream.writeEndElement();
}

void documentXML::addLetter(string display, int id, int x, int y, int width, int height)
{
  stream.writeStartElement("char");
  stream.writeAttribute("display", QString::fromStdString(display));
  stream.writeAttribute("id", QString::number(id));
  stream.writeAttribute("x", QString::number(x));
  stream.writeAttribute("y", QString::number(y));
  stream.writeAttribute("width", QString::number(width));
  stream.writeAttribute("height", QString::number(height));
  stream.writeEndElement();
}

//sauvegarde du xml dans le fichier donné en paramètre
bool documentXML::write(const string& fileName)
{
  QFile file(QString::fromStdString(fileName));
  
  if(!file.open(QFile::WriteOnly)){
    return false;
  }
  else{
    QTextStream txt(&file);
    txt<<xml;
  }
  return true;
}
