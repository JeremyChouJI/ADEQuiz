#include "production/ProductionLine.h"

#include <cassert>
#include <stdexcept>

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

    return 0;
}
