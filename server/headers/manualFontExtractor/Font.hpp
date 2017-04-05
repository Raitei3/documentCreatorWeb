#ifndef FONT_H
#define FONT_H

/**
 * \file Font.hpp
 * \brief Font of the user
 * \author  T.Autret, A.Boschet, T.Labrousse, R.Tisné
 * \version 1.0
 * \date Mars 2016
 *
 * Font generate by the labeling made by the user
 *
 */


#include <string.h> 
#include <iostream>
#include "Character.hpp"

class Font
{
public:
    /*!
     *  \brief Constructor of the Font class
     */
    Font();

     /*!
     *  \brief set the name of the font
     *
     *  Set accessor for name attribute
     * 
     *  \param n : the name as string
     */
    void setName(const std::string &n);

    /*!
     *  \brief get the name of the font
     *
     * Get accessor for name attribute
     * 
     *  \return the name as string
     */
    std::string getName() const;

    /*!
     *  \brief add character to the font
     *
     *  \param c: the c as Character
     */
    void addCharacter(const Character &c);

    /*!
     *  \brief Verify if character is in Font
     * 
     *  \param c: the c as character
     *
     *  \return the index as integer
     */
    int indexOfCharacter(const std::string &c);

    /*!
     *  \brief Verify if connected component is in Font
     *
     *  \param idCC : the idCC as integer
     *  \param idLine : the idLine as integer
     * 
     *  \return the index as integer
     */
    int indexOfCharacterForCC(int idCC, int idLine);

    /*!
     *  \brief get the copy of index of ConnectedComponent
     *
     * Get accessor for ConnectedComponent
     *
     *  \param id : the id as integer;
     * 
     *  \return the copy as pointer
     */
    Character* characterAtIndex(int id);

  const Character* characterAtIndex(int id) const;

  
    /*!
     *  \brief Remove character of Font
     *
     *  \param id : the id as interger
     */
    void removeCharacter(int id);

    /*!
     *  \brief Count number of character of Font
     *
     * Get accessor for size attribute
     * 
     *  \return the size as integer
     */
    int countCharacter() const;
private:
    //!@brief filename of the font
   	std::string m_name;
    
    //!@brief vector of Character of the font
   	std::vector<Character> m_listCharacter;

};

#endif // FONT_H
