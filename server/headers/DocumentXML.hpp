#ifndef DOCUMENTXML_HPP
#define DOCUMENTXML_HPP

#include <string>
#include <QXmlStreamWriter>
#include <QString>

class documentXML{
public:
    documentXML();
    
    void init(int widthDoc, int heightDoc, std::string fontName, std::string backgroundName);
    void openBlock(int x, int y, int width, int height);
    void closeBlock();
    void addLetter(std::string display, int id, int x, int y, int width, int height);
    void close();
    bool write(const std::string& file);

private:
    QXmlStreamWriter stream;
    QString xml;
};

#endif
