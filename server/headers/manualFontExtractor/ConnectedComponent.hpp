#ifndef CONNECTEDCOMPONENT_H
#define CONNECTEDCOMPONENT_H

/**
 * \file ConnectedComponent.hpp
 * \brief Component on the image
 * \author  T.Autret, A.Boschet, T.Labrousse, R.Tisné
 * \version 1.0
 * \date Mars 2016
 *
 * Component on the image
 *
 */

#include "Character.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class ConnectedComponent
{
public:
    /*!
     *  \brief Constructor of the Connected Component class
     */
    ConnectedComponent();

    /*!
     *  \brief Constructor of the Connected Component class
     *  \param vec : the vec as copy of vector of Point
     */
    explicit ConnectedComponent(const std::vector<cv::Point> &vec);

    /*!
     *  \brief get the List point of the connected component
     *
     *  Get accessor for list point attribute
     * 
     *  \return the list point as vector of point
     */
    std::vector<cv::Point> getListPoint() const;

    /*!
     *  \brief set the baseline of connected component
     *
     *  Set accessor of baseline attribute
     * 
     *  \param y : the y as integer
     */
    void setBaseline(int y);

    /*!
     *  \brief get the baseline of the connected component
     *
     *  Get accessor for baseline attribute
     * 
     *  \return the baseline of the connected component
     */
    int getBaseline() const;

    /*!
     *  \brief Bool to verify if connected component had to a line
     *
     *  \return the inline as bool
     */
    bool getInline() const;

    /*!
     *  \brief set the inline of the connected component
     *
     *  set accessor for inline attribute
     * 
     *  \param x : the x as integer
     */
    void setInline(bool x);

    /*!
     *  \brief set the boundingbox of the connected component
     *
     *  set accessor for boundingbox attribute
     * 
     *  \param up : the up as integer
     *  \param down : the down as integer
     *  \param left : the left as integer
     *  \param right : the right as integer
     */
    void setBoundingBox(int up, int down, int left, int right);

    /*!
     *  \brief get the boundingbox of the connected component
     *
     *  get accessor for boundingbox attribute
     * 
     *  \return the boundingbox as Rect
     */
    cv::Rect getBoundingBox() const;
private:
   //!@brief vector of Point of the Connected Component
   std::vector<cv::Point> m_listPoint;

   //!@brief baseline of the Connected Component
   float m_baseline;

   //!@brief up of the Connected Component
   int m_up;

   //!@brief down of the Connected Component
   int m_down;

   //!@brief right of the Connected Component
   int m_right;

   //!@brief left of the Connected Component
   int m_left;

   //!@brief inline of the Connected Component
   bool m_inline = false;
};

#endif // CONNECTEDCOMPONENT_H
