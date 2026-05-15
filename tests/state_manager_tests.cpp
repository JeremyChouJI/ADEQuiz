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

    ProductionLine restoredLine;
    assert(stateManager.load(restoredLine));
    assert((restoredLine.getFlow() == std::vector<std::string>{"A", "B", "C", "B", "A"}));
    assert((restoredLine.getProducts() == std::vector<int>{3}));
    assert((restoredLine.getStationCounts() == std::map<std::string, int>{{"A", 2}, {"B", 1}, {"C", 1}}));
    assert(restoredLine.processMaterial(2) == 3);
    assert((restoredLine.getProducts() == std::vector<int>{3, 3}));
    assert((restoredLine.getStationCounts() == std::map<std::string, int>{{"A", 4}, {"B", 2}, {"C", 2}}));

    std::remove(savePath.c_str());
    ProductionLine missingLine;
    assert(!stateManager.load(missingLine));
    assert(!missingLine.hasFlow());
    assert(missingLine.getProducts().empty());
    assert(missingLine.getStationCounts().empty());

    {
        std::ofstream output(savePath);
        output << "{ invalid json";
    }
    ProductionLine corruptedLine;
    assert(!stateManager.load(corruptedLine));
    assert(!corruptedLine.hasFlow());
    assert(corruptedLine.getProducts().empty());
    assert(corruptedLine.getStationCounts().empty());

    {
        std::ofstream output(savePath);
        output << "{\n";
        output << "  \"flow\": [\"A\", \"D\"],\n";
        output << "  \"products\": [3],\n";
        output << "  \"counts\": {\"A\": 1}\n";
        output << "}\n";
    }
    ProductionLine invalidLine;
    assert(!stateManager.load(invalidLine));
    assert(!invalidLine.hasFlow());
    assert(invalidLine.getProducts().empty());
    assert(invalidLine.getStationCounts().empty());

    StateManager failingStateManager("C:\\tmp\\adequiz_missing_directory\\state.json");
    assert(!failingStateManager.save(line));

    std::remove(savePath.c_str());
    return 0;
}
