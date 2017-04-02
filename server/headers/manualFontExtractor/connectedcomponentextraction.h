#ifndef CONNECTEDCOMPONENTEXTRACTION_H
#define CONNECTEDCOMPONENTEXTRACTION_H

#include <deque>

#include <opencv2/core/core.hpp>

typedef std::vector<cv::Point> CC;
typedef std::vector<CC> CCs;

typedef struct {
    cv::Point pixel;
    int label;
} LabelledPixel;

class ConnectedComponentExtraction
{
public:
    ConnectedComponentExtraction();

    /**
       @brief extract all connected componenents from image @a input and fill @a ccs.
       
       @a connectivity must be 4 or 8.
     */
    static void extractAllConnectedComponents(const cv::Mat &input, CCs &ccs, int connectivity = 8);

private:
    static void extractConnectedComponent(cv::Mat &input, const cv::Point &seed, CC &cc, int connectivity);
};

#endif // CONNECTEDCOMPONENTEXTRACTION_H
