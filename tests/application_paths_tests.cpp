#include "app/ApplicationPaths.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <cassert>
#include <string>
#include <vector>

int main()
{
    const std::string statePath =
        app::stateFilePathNextToExecutable("portable/ADEQuiz.exe");

    assert(statePath == "portable/state.json");

#ifdef _WIN32
    const std::string windowsStatePath =
        app::stateFilePathNextToExecutable("C:\\Portable\\ADEQuiz\\ADEQuiz.exe");

    assert(windowsStatePath == "C:\\Portable\\ADEQuiz\\state.json");

    std::vector<char> buffer(MAX_PATH);
    const DWORD length = GetModuleFileNameA(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));

    assert(length > 0);
    assert(app::defaultStateFilePath() ==
        app::stateFilePathNextToExecutable(std::string(buffer.data(), length)));
#endif

    return 0;
}
