#ifndef BLEEDTHROUGH_HPP
#define BLEEDTHROUGH_HPP

#include <QDebug>
#include <QRgb>
#include <QThread>
#include <QProgressDialog>
#include <cassert>
#include <iostream>


/*
  @a imgVerso is positionned at (@a x, @a y) in @a imgRecto frame.

  @warning @a imgVerso is not transformed. In particular, it is considered already mirrored.

  If @æ nbThreads is negative a value is determined according to image size.
 */
extern QImage bleedThrough(const QImage &imgRecto, const QImage &imgVerso, int nbIter, int x=0, int y=0, int nbThreads=-1);

extern QImage bleedThrough(const QImage &originalRecto, const QImage &imgRecto, const QImage &imgVerso, int nbIter, int x=0, int y=0, int nbThreads=-1);



class BleedThrough
{

public:

  explicit BleedThrough(int nbIterations) :
    _nbIter(nbIterations)
  {}

  void setVerso(const QString &path);

public:

  virtual QImage apply();
  virtual QImage apply(QString path);
  void setVersoAndApply(const QString &path);


protected :
  QImage _verso;
  int _nbIter;

};

#endif // BLEEDTHROUGH_HPP
