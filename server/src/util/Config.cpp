
#include <iostream>
#include <fstream>
#include <regex>
#include <unistd.h>
#include "Config.hpp"

using namespace std;

Config::Config()
{
  pathList={"config.ini"};
  const char* home(getenv("HOME"));
  if(home != NULL){
    pathList.push_back(string(home) + "/.documentCreatorServer.ini");
  }
  pathList.push_back("/etc/documentCreatorServer.ini");
  reload();
}

void Config::reload()
{
  for(auto it=pathList.begin();it!=pathList.end();it++){
    ifstream stream(*it);
    if(stream){
      return reload(stream);
    }
  }
  loadDefault();
    
}

void Config::reload(std::istream& stream)
{
  //regex decomposition

  const string spaces("[[:space:]]*");
  const string comment(spaces+"([#;].*)?");
  const string variableName("[[:alpha:]][[:alnum:]]*");
  const string& sectionName(variableName);
  const string number("[[:digit:]]+");
  const string str("[^#;[:space:]]+");
    
  regex regexSection("(\\[" + sectionName + "\\])" + comment);
  regex rpropertieInt("(" + variableName + ")=(" + number + ")" + comment);
  regex rpropertieString("(" + variableName + ")=(" + str + ")" + comment);
  regex remptyLine(comment);


  string section("path");
  smatch propertie;
  string line;
  while(stream && !stream.eof()){
    getline(stream, line);
    if(regex_match(line, propertie, regexSection)){
      section = propertie[1].str().substr(1, propertie[1].str().size() - 2);
    }
    else if(regex_match(line, propertie, rpropertieInt)){
      string propertieName = section + '/' + static_cast<const string>(propertie[1]);
      int value = stoi(propertie[2]);
      intValues[propertieName] = value;
    }
    else if(regex_match(line, propertie, rpropertieString)){
      string propertieName = section + '/'+static_cast<const string>(propertie[1]);
      stringValues[propertieName] = propertie[2];
    }
  }
  updateEnv();
}

void Config::loadDefault()
{
  intValues.clear();
  stringValues = {{"path/rootPath", "./"}};
}

int Config::getIntValue(string var)
{
  return intValues[var];
}

string Config::getStringValue(string var)
{
  return stringValues[var];
}

void Config::updateEnv()
{
  if(chdir(stringValues["path/rootPath"].c_str()) == -1)
    perror(stringValues["path/rootPath"].c_str());
}
