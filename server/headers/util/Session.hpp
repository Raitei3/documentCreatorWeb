#ifndef SESSION_HPP
#define SESSION_HPP

/**
 * \file Session.hpp
 * \brief Determine a user session
 * \author  T.Autret, A.Boschet, T.Labrousse, R.Tisné
 * \version 1.0
 * \date Mars 2016
 *
 * Determine a user session
 *
 */


#include "Font.hpp"
#include "Image.hpp"
class Session
{
public:
    /*!
     *  \brief Constructor of the Session class
     *  \param f : filename of the image
     */
    explicit Session(const std::string &f);
    
    /*!
     *  \brief Copy constructor of the Session class
     *  \param session : instance to copy
     */
    Session(const Session& session);
    
    /*!
     *  \brief Session destructor
     */
    ~Session();
    
    /*!
     *  \brief get the file name of the image
     *
     *  Get accessor for filename attribute
     * 
     *  \return the filename as string
     */
    std::string getOriginalFileName() const;
   
    /*!
     *  \brief set the file name of the image
     *
     *  Set accessor for filename attribute
     * 
     *  \param f : the filename as string
     */
    void setOriginalFileName(const std::string &f);

    /*!
     *  \brief get the file name of the currently displayed image
     *
     *  Get accessor for filename attribute
     * 
     *  \return the filename as string
     */
    std::string getDisplayedFileName() const;
   
    /*!
     *  \brief set the file name of the currently diplayed image
     *
     *  Set accessor for filename attribute
     * 
     *  \param f : the filename as string
     */
    void setDisplayedFileName(const std::string &f);
   
    /*!
     *  \brief set the token of the session
     *
     *  Set accessor for token attribute
     * 
     *  \param the token as interger
     */
    void setToken(int t);

     /*!
     *  \brief get the token of the session
     *
     * Get accessor for token attribute
     * 
     *  \return the token as interger
     */
    int getToken() const;

     /*!
     *  \brief get an pointer on the image
     *  \return pointer on the image
     */
    Image* getImage();

    /*!
     *  \brief get an pointer on the font
     *  \return pointer on the font
     */
    Font* getFont();

    /*!
     * \brief save the image that we want to display
     */
    void saveDisplayedImage(const std::string &upload_dir);

private:
    //!@brief filename of the image
    std::string m_originalFileName;

    //!@brief filename of the currently displayed image
    std::string m_displayedFileName;

    //!@brief unique identifier to access to the session 
    int m_token;

    //!@brief unique font of the client
    Font* m_font;

    //!@brief unique image of the client
    Image* m_image;
};

#endif // SESSION_HPP