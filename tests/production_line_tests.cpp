#include "production/ProductionLine.h"

#include <cassert>
#include <limits>
#include <map>
#include <stdexcept>
#include <vector>

int main()
{
    ProductionLine line;

    assert(!line.hasFlow());

    bool threwWithoutFlow = false;
    try {
        line.processMaterial(10);
    } catch (const std::logic_error& error) {
        threwWithoutFlow = true;
        assert(std::string(error.what()) == "No processing flow configured. Please configure a flow first.");
    }
    assert(threwWithoutFlow);

    assert(line.setFlow("A->B"));
    assert(line.hasFlow());
    assert((line.getFlow() == std::vector<std::string>{"A", "B"}));
    assert(line.processMaterial(10) == 10);

    assert(line.setFlow("A->B->C->B->A"));
    assert(line.processMaterial(2) == 3);

    assert(line.setFlow("C->A"));
    assert(line.processMaterial(3) == 4);

    assert(line.setFlow("C->B->A"));
    assert(line.processMaterial(2) == 3);

    assert(line.setFlow("A->C->B"));
    assert(line.processMaterial(1) == 2);

    assert(!line.setFlow("A->E"));
    assert(line.hasFlow());
    assert((line.getFlow() == std::vector<std::string>{"A", "C", "B"}));
    assert(line.processMaterial(10) == 10);

    ProductionLine statsLine;
    assert(statsLine.getProducts().empty());
    assert(statsLine.getStationCounts().empty());

    bool statsThrewWithoutFlow = false;
    try {
        statsLine.processMaterial(5);
    } catch (const std::logic_error&) {
        statsThrewWithoutFlow = true;
    }
    assert(statsThrewWithoutFlow);
    assert(statsLine.getProducts().empty());
    assert(statsLine.getStationCounts().empty());

    assert(statsLine.setFlow("A->A"));
    assert(statsLine.processMaterial(1) == 3);
    assert((statsLine.getProducts() == std::vector<int>{3}));
    assert((statsLine.getStationCounts() == std::map<std::string, int>{{"A", 2}}));

    ProductionLine singleStationLine;
    assert(singleStationLine.setFlow("A"));
    assert(singleStationLine.hasFlow());
    assert(singleStationLine.processMaterial(10) == 11);
    assert((singleStationLine.getProducts() == std::vector<int>{11}));
    assert((singleStationLine.getStationCounts() == std::map<std::string, int>{{"A", 1}}));

    ProductionLine singleStationCLine;
    assert(singleStationCLine.setFlow("C"));
    assert(singleStationCLine.processMaterial(2) == 2);
    assert((singleStationCLine.getProducts() == std::vector<int>{2}));
    assert((singleStationCLine.getStationCounts() == std::map<std::string, int>{{"C", 1}}));

    ProductionLine singleStationDLine;
    assert(singleStationDLine.setFlow("D"));
    assert(singleStationDLine.processMaterial(3) == 4);
    assert(singleStationDLine.processMaterial(4) == 6);
    assert((singleStationDLine.getProducts() == std::vector<int>{4, 6}));
    assert((singleStationDLine.getStationCounts() == std::map<std::string, int>{{"D", 2}}));

    ProductionLine stationDRouteLine;
    assert(stationDRouteLine.setFlow("A->D->B"));
    assert(stationDRouteLine.processMaterial(1) == 3);
    assert((stationDRouteLine.getProducts() == std::vector<int>{3}));
    assert((stationDRouteLine.getStationCounts() == std::map<std::string, int>{{"A", 1}, {"B", 1}, {"D", 1}}));

    ProductionLine stationCSkipLine;
    assert(stationCSkipLine.setFlow("C->A"));
    assert(stationCSkipLine.processMaterial(2) == 2);
    assert((stationCSkipLine.getProducts() == std::vector<int>{2}));
    assert((stationCSkipLine.getStationCounts() == std::map<std::string, int>{{"C", 1}}));

    ProductionLine stationCNoSkipLine;
    assert(stationCNoSkipLine.setFlow("C->A"));
    assert(stationCNoSkipLine.processMaterial(3) == 4);
    assert((stationCNoSkipLine.getProducts() == std::vector<int>{4}));
    assert((stationCNoSkipLine.getStationCounts() == std::map<std::string, int>{{"A", 1}, {"C", 1}}));

    ProductionLine emptyRestoreLine;
    assert(emptyRestoreLine.restoreState({}, {}, {}));
    assert(!emptyRestoreLine.hasFlow());
    assert(emptyRestoreLine.getFlow().empty());
    assert(emptyRestoreLine.getProducts().empty());
    assert(emptyRestoreLine.getStationCounts().empty());

    ProductionLine zeroCountRestoreLine;
    assert(zeroCountRestoreLine.restoreState({"D"}, {}, {{"D", 0}}));
    assert((zeroCountRestoreLine.getFlow() == std::vector<std::string>{"D"}));
    assert(zeroCountRestoreLine.getProducts().empty());
    assert((zeroCountRestoreLine.getStationCounts() == std::map<std::string, int>{{"D", 0}}));

    ProductionLine integerLimitRestoreLine;
    assert(integerLimitRestoreLine.restoreState(
        {"C"},
        {std::numeric_limits<int>::min(), std::numeric_limits<int>::max()},
        {{"C", 0}}));
    assert((integerLimitRestoreLine.getProducts() == std::vector<int>{
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max()
    }));
    assert((integerLimitRestoreLine.getStationCounts() == std::map<std::string, int>{{"C", 0}}));

    ProductionLine stationAOverflowLine;
    assert(stationAOverflowLine.setFlow("A"));
    bool stationAOverflowThrew = false;
    try {
        stationAOverflowLine.processMaterial(std::numeric_limits<int>::max());
    } catch (const std::overflow_error& error) {
        stationAOverflowThrew = true;
        assert(std::string(error.what()) == "result is outside the supported integer range.");
    }
    assert(stationAOverflowThrew);
    assert(stationAOverflowLine.getProducts().empty());
    assert(stationAOverflowLine.getStationCounts().empty());

    ProductionLine stationBOverflowLine;
    assert(stationBOverflowLine.setFlow("B"));
    bool stationBOverflowThrew = false;
    try {
        stationBOverflowLine.processMaterial(std::numeric_limits<int>::min());
    } catch (const std::overflow_error& error) {
        stationBOverflowThrew = true;
        assert(std::string(error.what()) == "result is outside the supported integer range.");
    }
    assert(stationBOverflowThrew);
    assert(stationBOverflowLine.getProducts().empty());
    assert(stationBOverflowLine.getStationCounts().empty());

    ProductionLine stationDOddOverflowLine;
    assert(stationDOddOverflowLine.setFlow("D"));
    bool stationDOddOverflowThrew = false;
    try {
        stationDOddOverflowLine.processMaterial(std::numeric_limits<int>::max());
    } catch (const std::overflow_error& error) {
        stationDOddOverflowThrew = true;
        assert(std::string(error.what()) == "result is outside the supported integer range.");
    }
    assert(stationDOddOverflowThrew);
    assert(stationDOddOverflowLine.getProducts().empty());
    assert(stationDOddOverflowLine.getStationCounts().empty());

    ProductionLine stationDEvenOverflowLine;
    assert(stationDEvenOverflowLine.setFlow("D"));
    bool stationDEvenOverflowThrew = false;
    try {
        stationDEvenOverflowLine.processMaterial(std::numeric_limits<int>::max() - 1);
    } catch (const std::overflow_error& error) {
        stationDEvenOverflowThrew = true;
        assert(std::string(error.what()) == "result is outside the supported integer range.");
    }
    assert(stationDEvenOverflowThrew);
    assert(stationDEvenOverflowLine.getProducts().empty());
    assert(stationDEvenOverflowLine.getStationCounts().empty());

    ProductionLine partialOverflowLine;
    assert(partialOverflowLine.setFlow("C->A"));
    bool partialOverflowThrew = false;
    try {
        partialOverflowLine.processMaterial(std::numeric_limits<int>::max());
    } catch (const std::overflow_error&) {
        partialOverflowThrew = true;
    }
    assert(partialOverflowThrew);
    assert(partialOverflowLine.getProducts().empty());
    assert(partialOverflowLine.getStationCounts().empty());

    assert(statsLine.setFlow("A->B->C->B->A"));
    assert(statsLine.processMaterial(2) == 3);
    assert((statsLine.getProducts() == std::vector<int>{3, 3}));
    assert((statsLine.getStationCounts() == std::map<std::string, int>{{"A", 4}, {"B", 1}, {"C", 1}}));

    assert(!statsLine.setFlow("A->E"));
    assert((statsLine.getProducts() == std::vector<int>{3, 3}));
    assert((statsLine.getStationCounts() == std::map<std::string, int>{{"A", 4}, {"B", 1}, {"C", 1}}));

    return 0;
}
