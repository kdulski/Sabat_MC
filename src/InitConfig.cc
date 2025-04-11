#include "InitConfig.hh"

#include <sstream>
#include <fstream>
#include <map>

InitConfig* InitConfig::fInit = nullptr;

InitConfig::InitConfig() {}

InitConfig::~InitConfig() {}

InitConfig* InitConfig::getInstance()
{
  if(!fInit) {
    fInit = new InitConfig();
  }
  return fInit;
}

void InitConfig::Initialization()
{
  variables["filenameAddTimeAndSeed"] = "t"; //t-true, everything else false
  variables["includeAlphaDetection"] = "n"; //t - true, everything else false
}

void InitConfig::Read()
{
  G4String st;
  G4String db;

  G4String lineTemp;
  G4String firstChar;

  std::ifstream indata;
  indata.open(initName.c_str());

  if(!indata.is_open()) {
    std::cout << "No " << initName << " file in the directory with the exe file!" << std::endl;
  } else {
    std::cout << "Reading initialization variables from " << initName << std::endl;

    while(std::getline(indata, lineTemp)) {
      std::istringstream is(lineTemp);
      firstChar = lineTemp[0];
      if (firstChar != "/" && firstChar != "\n" && firstChar != " " && !lineTemp.empty()) {
        is >> st >> db;
        if(variables.find(st) != variables.end()) {
          variables[st] = db;
          std::cout << st << " " << variables[st] << std::endl;
        } else {
          std::cout << st << " not defined" << std::endl;
        }
      }
    }
  }
  indata.close();
}
