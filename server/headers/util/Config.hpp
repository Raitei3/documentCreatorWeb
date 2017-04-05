#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <map>
#include <vector>

class Config
{
public:
    Config();
    void reload();
    std::string getStringValue(std::string var);
    int getIntValue(std::string var);
  
private:
    void loadDefault();
    void reload(std::istream&);
    void updateEnv();
    std::vector<std::string> pathList;
    std::map<std::string, std::string> stringValues;
    std::map<std::string, int> intValues;
};

#endif //CONFIG_HPP
