#include "convertor.h"
#include <cassert>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

Convertor::Convertor()
{
}

cv::Mat Convertor::getCvMat(const QImage &src)
{
  //B:TODO:OPTIM: avoid conversion to QImage::Format_ARGB32 if in an other format !

  //QImage qimage = src.convertToFormat(QImage::Format_ARGB32);
  QImage qimage = src;
  if (src.format() != QImage::Format_ARGB32 && src.format() != QImage::Format_RGB32)
    qimage = src.convertToFormat(QImage::Format_ARGB32);

 
  cv::Mat mat = cv::Mat(qimage.height(), qimage.width(), CV_8UC4, (void *)qimage.constBits(), qimage.bytesPerLine() );
  cv::Mat mat2 = cv::Mat(mat.rows, mat.cols, CV_8UC3 );
  int from_to[] = { 0, 0,  1, 1,  2, 2 };
  cv::mixChannels(&mat, 1, &mat2, 1, from_to, 3);
  
  return mat2;
}

cv::Mat Convertor::binarizeOTSU(const cv::Mat &src)
{
  assert(src.type() == CV_8UC1);

  cv::Mat img_binarization;
  cv::threshold(src, img_binarization, 0, 255, cv::THRESH_OTSU); //B  
  return img_binarization;
}

QImage Convertor::getQImage(const cv::Mat &image){

  cv::Mat rgb;
  if (image.channels() == 1)
    cv::cvtColor(image, rgb, cv::COLOR_GRAY2RGB);
  else
    rgb = image;
  assert(rgb.channels() == 3 || rgb.channels() == 4);

  const int h = rgb.rows;
  const int w = rgb.cols;

  QImage qimg(w, h, QImage::Format_ARGB32);

  if (rgb.type() == CV_32FC3) {

    for (int i=0; i<h; ++i) {
      const float *src = rgb.ptr<float>(i);
      QRgb *dst = (QRgb *)qimg.scanLine(i);
      for (int j=0; j<w; ++j) {
	//suppose src in BGR format
	uchar b = *src;
	++src;
	uchar g = *src;
	++src;
	uchar r = *src;
	++src;
	//Are values in src in [0.f; 225.f] or [0.f; 1.f] ???

	*dst = qRgb(r, g, b);
	++dst;
      }
    }

  }
  else if (rgb.type() == CV_8UC3) {

    for (int i=0; i<h; ++i) {
      const uchar *src = rgb.ptr<uchar>(i);
      QRgb *dst = (QRgb *)qimg.scanLine(i);
      for (int j=0; j<w; ++j) {
	//suppose src in BGR format
	uchar b = *src;
	++src;
	uchar g = *src;
	++src;
	uchar r = *src;
	++src;
	//shoud we cv::saturate_cast() ???

	*dst = qRgb(r, g, b);
	++dst;
      }
    }
    
  }
  else if (rgb.type() == CV_8UC4) //For transparency
    {
      
      QImage alphaImg = qimg.alphaChannel();

      for (int i=0; i<h; ++i) {
	const uchar *src = rgb.ptr<uchar>(i);
	QRgb *dst = (QRgb *)qimg.scanLine(i);
	for (int j=0; j<w; ++j) {
	  //suppose src in BGR format
	  uchar b = *src;
	  ++src;
	  uchar g = *src;
	  ++src;
	  uchar r = *src;
	  ++src;
	  uchar a = *src;
	  ++src;
	  //shoud we cv::saturate_cast() ???
	  
	  *dst = qRgba(r, g, b, a);
	  ++dst;
	}
      }
         
    }
  else {
    std::cerr<<"ERROR: Convertor::getQImage() : unknown type \n";
    assert(false);
  }


  return qimg;
}
