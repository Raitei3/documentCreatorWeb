#include "StructureDetection.hpp"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/photo/photo.hpp>

#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include "baseline.hpp"
#include "binarization.hpp"

std::vector<cv::Rect> structureDetection::getBlocks(const cv::Mat &distanceMap, int char_height){
   // Then the images are binarized
   cv::Mat distance2 = cv::Mat(distanceMap.rows, distanceMap.cols, CV_8U);
   Binarization::binarize(distanceMap, distance2);

   cv::bitwise_not(distance2, distance2);

   // Then we close the image
   char_height /= 2;
   const cv::Mat element = cv::getStructuringElement( 2,
                                        cv::Size( 2*char_height + 1, 2*char_height+1 ),
                                        cv::Point( char_height, char_height ) );

   cv::dilate(distance2, distance2, element);
   cv::erode(distance2, distance2, element);

    // We find the connected blocks and draw them on the background
   std::vector<cv::Rect> blocks;
   std::vector<std::vector<cv::Point> > contours;
   cv::findContours(distance2.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_KCOS);

    for (size_t i=0; i<contours.size(); ++i) {
      const cv::Rect r = cv::boundingRect(contours[i]);
      if (r.area() > (distanceMap.cols * distanceMap.rows)*0.005) {
          blocks.push_back(r);
      }
    }

    return blocks;
}

#if 0

std::vector<cv::Rect> structureDetection::getWordBlocks(const cv::Mat &img_bin)
{
    tesseract::TessBaseAPI tess;

    tess.InitForAnalysePage();

    tess.SetImage((uchar*)img_bin.data, img_bin.cols, img_bin.rows, 1, img_bin.cols);

    tesseract::PageIterator *iter = tess.AnalyseLayout();
assert(iter);
    std::vector<cv::Rect> blocks;
    while (iter->Next(tesseract::RIL_WORD)) {
        int left, top, right, bottom;

        iter->BoundingBox(tesseract::RIL_WORD, &left, &top, &right, &bottom);
        blocks.push_back(cv::Rect(left, top, right-left, bottom-top));
    }

    return blocks;
}

#endif

int structureDetection::getCharacterHeight(const cv::Mat &img)
{
    return (int)Baseline::getCharacterHeight(img).first;
}

cv::Mat structureDetection::getDistanceMap(const cv::Mat &img, const cv::Mat &img_bin){


    cv::Mat distance = cv::Mat(img.rows, img.cols, CV_32SC1);

    // We compute the number of successive white pixels in the four directions for each pixel
    for (int row=0; row < img.rows; ++row)
        for (int col=0; col < img.cols; ++col) {

            int sum_h = 0;
            int sum_v = 0;

            // Horizontal
            // left
            for (int i=col; i>=0; --i) {
                if (img_bin.at<uchar>(row, i) == 0)
                    break;
                ++sum_h;
            }
            // right
            for (int i=col; i<img.cols; ++i) {
                if (img_bin.at<uchar>(row, i) == 0)
                    break;
                ++sum_h;
            }

            // Vertical
            // left
            for (int i=col; i>=0; --i) {
                if (img_bin.at<uchar>(i, col) == 0)
                    break;
                ++sum_v;
            }
            // right
            for (int i=col; i<img.rows; ++i) {
                if (img_bin.at<uchar>(i, col) == 0)
                    break;
                ++sum_v;
            }

            distance.at<int>(row, col) = sum_h*img.rows + sum_v*img.cols;
        }

    cv::Mat distance2 = cv::Mat(img.rows, img.cols, CV_8U);

    // We normalize to get an understandable image
    cv::normalize(distance, distance2, 0, 255, cv::NORM_MINMAX, CV_8U);

    return distance2;

}
