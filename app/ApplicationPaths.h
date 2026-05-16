#ifndef APP_APPLICATIONPATHS_H
#define APP_APPLICATIONPATHS_H

#include <string>

namespace app {

std::string stateFilePathNextToExecutable(const std::string& executablePath);
std::string defaultStateFilePath();

}

#endif
