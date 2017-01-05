#ifndef BINARIZATION_H
#define BINARIZATION_H

/**
 * \file Binarization.hpp
 * \brief Binarization operations
 * \author provided by Antoine BILLY
 * \version 1.0
 * \date Mars 2016
 *
 * Operations for binarize a image
 *
 */

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

enum NiblackVersion 
{
	NIBLACK=0,
    SAUVOLA,
    WOLFJOLION,
};

#define uget(x,y)    at<unsigned char>(y,x)
#define uset(x,y,v)  at<unsigned char>(y,x)=v;
#define fget(x,y)    at<float>(y,x)
#define fset(x,y,v)  at<float>(y,x)=v;

double calcLocalStats (cv::Mat &im, cv::Mat &map_m, cv::Mat &map_s, int winx, int winy);
void NiblackSauvolaWolfJolion (cv::Mat im, cv::Mat output, NiblackVersion version, double dR=128, int winx=40, int winy=40, double k=0.34);

#endif // BINARIZATION_H
