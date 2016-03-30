#ifndef CONNECTEDCOMPONENT_H
#define CONNECTEDCOMPONENT_H

#include "Character.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class ConnectedComponent
{
public:
    ConnectedComponent();
    explicit ConnectedComponent(std::vector<cv::Point> vec);
    const std::vector<cv::Point> getListPoint() const;
    void setBaseline(int x);
    const int getBaseline() const;
    const bool getInline() const;
    void setInline(bool x);
    void setBoundingBox(int up, int down, int left, int right);
    cv::Rect getBoundingBox();
private:
   std::vector<cv::Point> m_listPoint;
   float m_baseline;
   int m_up;
   int m_down;
   int m_right;
   int m_left;
   bool m_inline = false;
};

#endif // CONNECTEDCOMPONENT_H
