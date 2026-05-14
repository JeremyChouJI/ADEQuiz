#include "station/Stations.h"
#include "station/StationFactory.h"

#include <cassert>
#include <memory>
#include <string>

int main()
{
    StationA stationA;
    StationB stationB;
    StationC stationC;

    assert(stationA.name() == "A");
    ProcessResult resultA = stationA.process(10);
    assert(resultA.value == 11);
    assert(resultA.nextStepOffset == 1);

    assert(stationB.name() == "B");
    ProcessResult resultB = stationB.process(10);
    assert(resultB.value == 9);
    assert(resultB.nextStepOffset == 1);

    assert(stationC.name() == "C");
    ProcessResult oddResultC = stationC.process(5);
    assert(oddResultC.value == 5);
    assert(oddResultC.nextStepOffset == 1);

    ProcessResult evenResultC = stationC.process(6);
    assert(evenResultC.value == 6);
    assert(evenResultC.nextStepOffset == 2);

    assert(StationFactory::isSupportedStation("A"));
    assert(StationFactory::isSupportedStation("B"));
    assert(StationFactory::isSupportedStation("C"));
    assert(!StationFactory::isSupportedStation("D"));
    assert(!StationFactory::isSupportedStation(""));

    std::unique_ptr<ProcessingStation> createdA = StationFactory::createStation("A");
    std::unique_ptr<ProcessingStation> createdB = StationFactory::createStation("B");
    std::unique_ptr<ProcessingStation> createdC = StationFactory::createStation("C");
    std::unique_ptr<ProcessingStation> createdD = StationFactory::createStation("D");
    std::unique_ptr<ProcessingStation> createdEmpty = StationFactory::createStation("");

    assert(createdA != nullptr);
    assert(createdA->name() == "A");

    assert(createdB != nullptr);
    assert(createdB->name() == "B");

    assert(createdC != nullptr);
    assert(createdC->name() == "C");

    assert(createdD == nullptr);
    assert(createdEmpty == nullptr);

    return 0;
}
