#include "production/ProductionLine.h"
#include "state/StateManager.h"

#include <nlohmann/json.hpp>

#include <cassert>
#include <cstdio>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace {
std::string readFile(const std::string& path)
{
    std::ifstream input(path);
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

nlohmann::json readJsonFile(const std::string& path)
{
    return nlohmann::json::parse(readFile(path));
}

void writeFile(const std::string& path, const std::string& text)
{
    std::ofstream output(path);
    output << text;
}
}

int main()
{
    const std::string savePath = "adequiz_state_manager_test.json";
    std::remove(savePath.c_str());

    ProductionLine line;
    assert(line.setFlow("A->D->B->C"));
    assert(line.processMaterial(2) == 3);

    StateManager stateManager(savePath);
    assert(stateManager.save(line));

    const nlohmann::json savedJson = readJsonFile(savePath);
    assert((savedJson.at("flow").get<std::vector<std::string>>() == std::vector<std::string>{"A", "D", "B", "C"}));
    assert((savedJson.at("products").get<std::vector<int>>() == std::vector<int>{3}));
    assert((savedJson.at("counts").get<std::map<std::string, int>>() == std::map<std::string, int>{{"A", 1}, {"B", 1}, {"C", 1}, {"D", 1}}));

    ProductionLine restoredLine;
    assert(stateManager.load(restoredLine));
    assert((restoredLine.getFlow() == std::vector<std::string>{"A", "D", "B", "C"}));
    assert((restoredLine.getProducts() == std::vector<int>{3}));
    assert((restoredLine.getStationCounts() == std::map<std::string, int>{{"A", 1}, {"B", 1}, {"C", 1}, {"D", 1}}));
    assert(restoredLine.processMaterial(2) == 3);
    assert((restoredLine.getProducts() == std::vector<int>{3, 3}));
    assert((restoredLine.getStationCounts() == std::map<std::string, int>{{"A", 2}, {"B", 2}, {"C", 2}, {"D", 2}}));

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
        output << "  \"flow\": [\"A\", \"B\"],\n";
        output << "  \"counts\": {\"A\": 1}\n";
        output << "}\n";
    }
    ProductionLine missingFieldLine;
    assert(!stateManager.load(missingFieldLine));
    assert(!missingFieldLine.hasFlow());
    assert(missingFieldLine.getProducts().empty());
    assert(missingFieldLine.getStationCounts().empty());

    {
        std::ofstream output(savePath);
        output << "{\n";
        output << "  \"flow\": [\"A\", \"B\"],\n";
        output << "  \"products\": \"not an array\",\n";
        output << "  \"counts\": {\"A\": 1}\n";
        output << "}\n";
    }
    ProductionLine wrongTypeLine;
    assert(!stateManager.load(wrongTypeLine));
    assert(!wrongTypeLine.hasFlow());
    assert(wrongTypeLine.getProducts().empty());
    assert(wrongTypeLine.getStationCounts().empty());

    {
        std::ofstream output(savePath);
        output << "{\n";
        output << "  \"flow\": [\"A\", \"E\"],\n";
        output << "  \"products\": [3],\n";
        output << "  \"counts\": {\"A\": 1}\n";
        output << "}\n";
    }
    ProductionLine invalidLine;
    assert(!stateManager.load(invalidLine));
    assert(!invalidLine.hasFlow());
    assert(invalidLine.getProducts().empty());
    assert(invalidLine.getStationCounts().empty());

    {
        std::ofstream output(savePath);
        output << "{\n";
        output << "  \"flow\": [\"A\", \"B\"],\n";
        output << "  \"products\": [3],\n";
        output << "  \"counts\": {\"A\": -1}\n";
        output << "}\n";
    }
    ProductionLine negativeCountLine;
    assert(!stateManager.load(negativeCountLine));
    assert(!negativeCountLine.hasFlow());
    assert(negativeCountLine.getProducts().empty());
    assert(negativeCountLine.getStationCounts().empty());

    writeFile(savePath, "{\n  \"flow\": [],\n  \"products\": [],\n  \"counts\": {}\n}\n");
    ProductionLine emptyStateLine;
    assert(stateManager.load(emptyStateLine));
    assert(!emptyStateLine.hasFlow());
    assert(emptyStateLine.getProducts().empty());
    assert(emptyStateLine.getStationCounts().empty());

    writeFile(savePath, "{\n  \"flow\": [\"A\"],\n  \"products\": [],\n  \"counts\": {\"E\": 1}\n}\n");
    ProductionLine unknownCountStationLine;
    assert(!stateManager.load(unknownCountStationLine));
    assert(!unknownCountStationLine.hasFlow());
    assert(unknownCountStationLine.getProducts().empty());
    assert(unknownCountStationLine.getStationCounts().empty());

    writeFile(savePath, "{\n  \"flow\": [\"A\"],\n  \"products\": [],\n  \"counts\": {\"A\": 0}\n}\n");
    ProductionLine zeroCountLine;
    assert(stateManager.load(zeroCountLine));
    assert((zeroCountLine.getFlow() == std::vector<std::string>{"A"}));
    assert(zeroCountLine.getProducts().empty());
    assert((zeroCountLine.getStationCounts() == std::map<std::string, int>{{"A", 0}}));

    writeFile(savePath, "{\n  \"flow\": [\"A\"],\n  \"products\": [2147483648],\n  \"counts\": {\"A\": 1}\n}\n");
    ProductionLine outOfRangeProductLine;
    assert(!stateManager.load(outOfRangeProductLine));
    assert(!outOfRangeProductLine.hasFlow());
    assert(outOfRangeProductLine.getProducts().empty());
    assert(outOfRangeProductLine.getStationCounts().empty());

    writeFile(savePath, "{\n  \"flow\": \"A->B\",\n  \"products\": [],\n  \"counts\": {}\n}\n");
    ProductionLine wrongFlowTypeLine;
    assert(!stateManager.load(wrongFlowTypeLine));
    assert(!wrongFlowTypeLine.hasFlow());
    assert(wrongFlowTypeLine.getProducts().empty());
    assert(wrongFlowTypeLine.getStationCounts().empty());

    writeFile(savePath, "{\n  \"flow\": [\"A\"],\n  \"products\": [],\n  \"counts\": {\"A\": 0},\n  \"version\": 1\n}\n");
    ProductionLine extraFieldLine;
    assert(stateManager.load(extraFieldLine));
    assert((extraFieldLine.getFlow() == std::vector<std::string>{"A"}));
    assert(extraFieldLine.getProducts().empty());
    assert((extraFieldLine.getStationCounts() == std::map<std::string, int>{{"A", 0}}));

    writeFile(savePath, "");
    ProductionLine emptyFileLine;
    assert(!stateManager.load(emptyFileLine));
    assert(!emptyFileLine.hasFlow());
    assert(emptyFileLine.getProducts().empty());
    assert(emptyFileLine.getStationCounts().empty());

    StateManager failingStateManager("C:\\tmp\\adequiz_missing_directory\\state.json");
    assert(!failingStateManager.save(line));

    std::remove(savePath.c_str());
    return 0;
}
