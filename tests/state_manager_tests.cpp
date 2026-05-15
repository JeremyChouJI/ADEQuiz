#include "production/ProductionLine.h"
#include "state/StateManager.h"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

namespace {
std::string readFile(const std::string& path)
{
    std::ifstream input(path);
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

bool contains(const std::string& text, const std::string& expected)
{
    return text.find(expected) != std::string::npos;
}
}

int main()
{
    const std::string savePath = "adequiz_state_manager_test.json";
    std::remove(savePath.c_str());

    ProductionLine line;
    assert(line.setFlow("A->B->C->B->A"));
    assert(line.processMaterial(2) == 3);

    StateManager stateManager(savePath);
    assert(stateManager.save(line));

    const std::string json = readFile(savePath);
    assert(contains(json, "\"flow\": [\"A\", \"B\", \"C\", \"B\", \"A\"]"));
    assert(contains(json, "\"products\": [3]"));
    assert(contains(json, "\"counts\": {"));
    assert(contains(json, "\"A\": 2"));
    assert(contains(json, "\"B\": 1"));
    assert(contains(json, "\"C\": 1"));

    StateManager failingStateManager("C:\\tmp\\adequiz_missing_directory\\state.json");
    assert(!failingStateManager.save(line));

    std::remove(savePath.c_str());
    return 0;
}
