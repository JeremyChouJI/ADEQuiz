#include "production/ProductionLine.h"

#include <cassert>
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
    assert(line.processMaterial(10) == 10);

    assert(line.setFlow("A->B->C->B->A"));
    assert(line.processMaterial(2) == 3);

    assert(line.setFlow("C->A"));
    assert(line.processMaterial(3) == 4);

    assert(line.setFlow("C->B->A"));
    assert(line.processMaterial(2) == 3);

    assert(line.setFlow("A->C->B"));
    assert(line.processMaterial(1) == 2);

    assert(!line.setFlow("A->D"));
    assert(line.hasFlow());
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

    assert(statsLine.setFlow("A->B->C->B->A"));
    assert(statsLine.processMaterial(2) == 3);
    assert((statsLine.getProducts() == std::vector<int>{3, 3}));
    assert((statsLine.getStationCounts() == std::map<std::string, int>{{"A", 4}, {"B", 1}, {"C", 1}}));

    assert(!statsLine.setFlow("A->D"));
    assert((statsLine.getProducts() == std::vector<int>{3, 3}));
    assert((statsLine.getStationCounts() == std::map<std::string, int>{{"A", 4}, {"B", 1}, {"C", 1}}));

    return 0;
}
