#include "app/ApplicationPaths.h"

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#endif

#include <cerrno>
#include <vector>

namespace {
std::string currentWorkingDirectory()
{
    std::vector<char> buffer(1024);

    while (true) {
        errno = 0;
#ifdef _WIN32
        if (_getcwd(buffer.data(), static_cast<int>(buffer.size())) != nullptr) {
            return buffer.data();
        }
#else
        if (getcwd(buffer.data(), buffer.size()) != nullptr) {
            return buffer.data();
        }
#endif

        if (errno != ERANGE) {
            return "";
        }

        buffer.resize(buffer.size() * 2);
    }
}

std::string joinPath(const std::string& directory, const std::string& fileName)
{
    if (directory.empty()) {
        return fileName;
    }

    const char lastCharacter = directory[directory.size() - 1];
    if (lastCharacter == '/' || lastCharacter == '\\') {
        return directory + fileName;
    }

#ifdef _WIN32
    return directory + "\\" + fileName;
#else
    return directory + "/" + fileName;
#endif
}
}

namespace app {

std::string stateFilePathNextToExecutable(const std::string& executablePath)
{
    const std::string::size_type lastSeparator =
        executablePath.find_last_of("/\\");

    if (lastSeparator == std::string::npos) {
        return "state.json";
    }

    return executablePath.substr(0, lastSeparator + 1) + "state.json";
}

std::string defaultStateFilePath()
{
#ifdef _WIN32
    std::vector<char> buffer(MAX_PATH);

    while (true) {
        const DWORD length = GetModuleFileNameA(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
        if (length == 0) {
            break;
        }

        if (length < buffer.size()) {
            return stateFilePathNextToExecutable(std::string(buffer.data(), length));
        }

        buffer.resize(buffer.size() * 2);
    }
#endif

    return joinPath(currentWorkingDirectory(), "state.json");
}

}
