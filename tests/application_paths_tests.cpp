#include "app/ApplicationPaths.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <cassert>
#include <string>
#include <vector>

int main()
{
    const std::string fileNameOnlyStatePath =
        app::stateFilePathNextToExecutable("ADEQuiz.exe");

    assert(fileNameOnlyStatePath == "state.json");

    const std::string emptyExecutableStatePath =
        app::stateFilePathNextToExecutable("");

    assert(emptyExecutableStatePath == "state.json");

    const std::string statePath =
        app::stateFilePathNextToExecutable("portable/ADEQuiz.exe");

    assert(statePath == "portable/state.json");

    const std::string nestedForwardSlashStatePath =
        app::stateFilePathNextToExecutable("portable/tools/ADEQuiz.exe");

    assert(nestedForwardSlashStatePath == "portable/tools/state.json");

#ifdef _WIN32
    const std::string windowsRootStatePath =
        app::stateFilePathNextToExecutable("C:\\ADEQuiz.exe");

    assert(windowsRootStatePath == "C:\\state.json");

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
