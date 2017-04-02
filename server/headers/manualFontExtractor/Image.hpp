#ifndef IMAGE_H
#define IMAGE_H

/**
 * \file Image.hpp
 * \brief Image to be processed
 * \author  T.Autret, A.Boschet, T.Labrousse, R.Tisné
 * \version 1.0
 * \date Mars 2016
 *
 * Image to be processed
 *
 */


#include "Line.hpp"

class Image
{
public:
    
    /*!
     *  \brief Constructor of the Image class
     *  \param path : path of the image
     */
    explicit Image(const std::string &path);

    /*!
     *  \brief get the matrice of the image
     *
     *  Get accessor for matrice attribute
     * 
     *  \return the image as matrice
     */
    cv::Mat getMat() const;   

    /*!
     *  \brief set the matrice of the image
     *
     *  set accessor for matrice attribute
     *
     *  \param mat : the image as matrice
     */
    void setMat(const cv::Mat &mat);

    /*!
     *  \brief get the image of Connected component for extraction of font 
     *
     *  Get accessor for data from Component attribute
     *
     *  \param index : the index as integer
     *  \param lineId : the lineId as integer
     *
     *  \return the data from component as string
     */
    std::string extractDataFromComponent(int index, int lineId) const; 

    /*!
     *  \brief Create the mask and attribute each connected component to a line
     */
    void ComputeMask();

    /*!
     *  \brief Binarizing the image
     * 
     *  \return the image binarized as matrice 
     */
    cv::Mat binarizeImage() const;

    /*!
     *  \brief Extraction connected components of Image
     * 
     *  \return the list of connected components as vector
     */
    static cv::vector<ConnectedComponent> extractComposentConnectImage(cv::Mat img);

    /*!
     *  \brief Verify if middle of connected component is in line
     * 
     *  \return the middle as bool
     */
    bool inMiddle(cv::Rect bb1, cv::Rect bb2) const;

    /*!
     *  \brief Creation Json informations about each connected component
     * 
     *  \return the json as string
     */
    std::string jsonBoundingRect();

    /*!
     *  \brief Creation Json informations about each baseline of Line
     * 
     *  \return the json as string
     */
    std::string jsonBaseline();

    /*!
     *  \brief Verify if line is valid
     *  
     *  \param line : the line as integer
     *
     *  \return the validIdLine as integer
     */
    int isValidIdLine(int line) const;

    /*!
     *  \brief Verify if line is valid and cc is valid
     *  
     *  \param line : the line as integer
     *  \param cc : the cc as integer
     *
     *  \return the validIdCC as integer
     */
    int isValidIdCC(int line, int cc) const;
    
    /*!
     *  \brief set BoundingBox informations of Connected component
     *
     *  Set accessor for boundingbox attribute
     * 
     *  \param index : the index as int
     *  \param line : the line as int
     *  \param up : the up as int
     *  \param down : the down as int
     *  \param left : the left as int
     *  \param right : the right as int
     */
    void setBoundingBoxAtIndex(int index,int line, int up, int down, int left, int right);

    /*!
     *  \brief set baseline informations of ConnectedComponent
     *
     *  Set accessor for baseline of Connected component attribute
     * 
     *  \param index : the index as int
     *  \param line : the line as int
     *  \param value : the value as int
     */
    void setBaselineAtIndex(int index,int line, int value);   

    /*!
     *  \brief set baseline informations of Line
     *
     *  Set accessor for baseline of Line attribute
     * 
     *  \param id : the id as string
     *  \param value : the value as string
     */ 
    void setBaselineForLine(int id, int value);
    
    /*!
     *  \brief get Median of CharacterHeight of Image
     *
     *  get accessor for CharacterHeight attribute
     * 
     *  \param img: the img as matrice
     *
     *  \return the character height as integer
     */
    static int getCharacterHeight(const cv::Mat &img);

    /*!
     *  \brief get Bounding box of ConnectedComponent
     *
     *  get accessor for boundingbox attribute
     * 
     *  \param index: the index as integer
     *  \param line: the line as integer
     *
     *  \return the boundingbox as Rect
     */
    cv::Rect getBoundingBoxAtIndex(int index, int line) const;  

    /*!
     *  \brief get baseline of ConnectedComponent
     *
     *  get accessor for baseline attribute
     * 
     *  \param index: the index as integer
     *  \param line: the line as integer
     *
     *  \return the baseline as integer
     */ 
    int getBaselineAtIndex(int index,int line);

    /*!
     *  \brief get positons of connected component in vector
     *
     *  get accessor for connected component attribute
     * 
     *  \param index: the index as integer
     *  \param line: the line as integer
     *
     *  \return the connectedcomponent as ConnectedComponent
     */
    ConnectedComponent getConnectedComponnentAt(int index, int line) const; 

    /*!
     *  \brief remove the connected component
     * 
     *  \param index: the index as integer
     *  \param line: the line as integer
     *
     */
    void removeConnectedComponentAt(int index, int line);

    /*!
     *  \brief get the lines present on image
     *
     *  get accessor for m_listLine attribute
     *
     *  \return the list line as vector of Line
     */
    std::vector<Line> getListLine();

private:
//!@brief Mat of the image uploaded
 cv::Mat m_img;
 
//!@brief vector of Line
 std::vector<Line> m_listLine;
};

#endif // IMAGE_H
