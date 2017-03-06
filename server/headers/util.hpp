#ifndef UTIL_HPP
#define UTIL_HPP

class MyDynamicPage : public DynamicPage
{
};

/* split the string @a s */
int split(std::vector<std::string>& v, const std::string &s, char separateur);

/*
 * \brief Function use when the service is closed and killed the active session
 *
 * \param dummy : the dummy as integer
 */
void exitFunction( int /*dummy*/ );

/*
 * \brief Verify if the format's image uploaded is supported
 *
 * \param &filenane : the filename as string
 *
 * \return a bool
 */
inline
bool isFormatSupported( const std::string &filename);

/*
 * Auxialiry function to save one instance of character to XML file
 *
 */
inline
void
saveInstanceToXml(std::ostream &xmlDocument, int id, int width, int height, const std::string &data);

/*
 * Make data for space character (that is, transparent white square) as a string
 */
inline
std::string
makeSpaceCharacterData(int width, int height);

/*
 * \brief Extract all informations about the font
 *
 * \param sessionindex : the index as integer
 * \param fontname : the fontname as string
 *
 * \return a string
 */
std::string extractFontInOf(int sessionIndex, const std::string &fontName);

/*
 * \brief Generate a random name for image uploaded
 *
 * \param extension : the extension as string
 *
 * \return a string
 */
std::string gen_random(const std::string &extension);

/*
 * \brief Creater User session when image was uploading
 *
 * \param filenane : the filename as string
 * \param *request : the request as HttpRequest
 *
 * \return a string JSON
 */
std::string InitiateSession(const std::string &filename, HttpRequest* /*request*/);

/*
 * \brief Verify if token is valid
 *
 * \param token : the token as integer
 *
 * \return a integer
 */
int getActiveSessionFromToken(int token);

#endif //UTIL_HPP
