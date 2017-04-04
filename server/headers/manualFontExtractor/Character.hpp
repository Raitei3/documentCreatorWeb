#ifndef CHARACTER_H
#define CHARACTER_H

/**
 * \file Character.hpp
 * \brief Character of the font
 * \author  T.Autret, A.Boschet, T.Labrousse, R.Tisné
 * \version 1.0
 * \date Mars 2016
 *
 * Character of the font
 *
 */

#include <opencv2/core/core.hpp>
#include <iostream>

class Character
{
public:
    /*!
     *  \brief Constructor of the Character class
     */
    Character();

    /*!
     *  \brief Constructor of the Character class
     *  \param l : filename of the character
     */
    explicit Character(const std::string &l);

    /*!
     *  \brief get the label of the character
     *
     *  Get accessor for label attribute
     * 
     *  \return the label of character
     */
    std::string getLabel() const;

    /*!
     *  \brief set the baseline of the character
     *
     *  set accessor for baseline attribute
     * 
     *  \param x : the x as integer
     */
    void setBaseline(int x);

    /*!
     *  \brief get the baseline of the character
     *
     *  Get accessor for baseline attribute
     * 
     *  \return the baseline of character
     */
    int getBaseline() const;

    /*!
     *  \brief get the number of character in  m_idComponentsConnexes
     *
     *  Get accessor for size attribute
     * 
     *  \return the size as integer
     */
    int countComponent() const;

    /*!
     *  \brief add component to m_idComponentsConnexes
     * 
     *  \param idCC : the idCC as integer
     *  \param idLine : the idLine as integer
     */
    void addComponent(int idCC, int idLine);

    /*!
     *  \brief remove component from m_idComponentsConnexes
     * 
     *  \param idCC : the idCC as integer
     *  \param idLine : the idLine as integer
     */
    void removeComponent(int idCC, int idLine);

    /*!
     *  \brief verify component is in m_idComponentsConnexes
     * 
     *  \param idCC : the idCC as integer
     *  \param idLine : the idLine as integer
     *
     *  \return the hasComponent as bool
     */
    bool hasComponent(int idCC, int idLine) const;

    /*!
     *  \brief get index informations to m_idComponentsConnexes
     * 
     *  \param index: the index as integer
     *
     *  \return the index of character
     */
    std::pair<int,int> getIdComponentAtIndex(int index) const;

    friend bool operator==(const Character &c1, const Character &c2);
  
private:
    //!@brief label of the Character
    std::string m_label;

    //!@brief baseline of the Character
    int m_baseline;

    //!@brief vector of pair integer of the Character
    std::vector<std::pair<int,int>> m_idComponentsConnexes;
};

#endif // CHARACTER__H
