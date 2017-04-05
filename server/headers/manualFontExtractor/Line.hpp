#ifndef LINE_H
#define LINE_H

/**
 * \file Line.hpp
 * \brief Lines in this picture
 * \author  T.Autret, A.Boschet, T.Labrousse, R.Tisné
 * \version 1.0
 * \date Mars 2016
 *
 * Lines in this picture
 *
 */


#include "ConnectedComponent.hpp"

class Line
{
public:
  /*!
     *  \brief Constructor of the Line class
     */
    Line();

    /*!
     *  \brief set the baseline of Line
     *
     *  Set accessor for baseline attribute
     * 
     *  \param x : the baseline as integer
     */
  	void setBaseline(int x);

    /*!
     *  \brief get the baseline of Line
     *
     * Get accessor for baseline
     * 
     *  \return the baseline as integer
     */    
  	int getBaseline() const;

    /*!
     *  \brief get the vector of ConnectedComponent of a Line
     *
     * Get accessor for token vector of ConnectedComponent
     * 
     *  \return the ListCC as vector of Connected Component 
     */
  	std::vector<ConnectedComponent> getListCC() const;

    /*!
     *  \brief add a pointer of a ConnectedComponent to a the vector
     *
     *  Push pointer of Connected Component
     * 
     *  \param cc: the cc as pointer of ConnectedComponent
     */
  	void addCC(const ConnectedComponent &cc);

    /*!
     *  \brief get the ConnectedComponent of a Line
     *
     *  Get accessor for connected component with a index
     *  \param index: the index as integer
     *  \return the Connected Component as connected Component
     */
  	ConnectedComponent getConnectedComponentAtIndex(int index) const;

    /*!
     *  \brief set informations about boundingbox of connectedComponent
     * 
     *  \param index : the index as integer 
     *  \param up : the up as integer 
     *  \param down : the down as integer 
     *  \param left : the left as integer 
     *  \param right : the right as integer
     */
  	void setBoundingBoxAtIndex(int index, int up, int down, int left, int right);

    /*!
     *  \brief set the baseline to value at Line index
     * 
     *  \param index : the index as integer 
     *  \param value : the value as integer
     */
    void setBaselineAtIndex(int index, int value);

    /*!
     *  \brief remove connected component
     * 
     *  \param index : the index as integer 
     */
  	void removeConnectedComponentAt(int index);

    /*!
     *  \brief Set the baseline for each line with median of vector Of Connected Component
     */
    void computeBaseLine();

private:
  //!@brief vector of Connected Component of the Line
  std::vector<ConnectedComponent> m_ListCC;

  //!@brief Baseline of the Line
  int m_baseline;
};

#endif // LINE__H