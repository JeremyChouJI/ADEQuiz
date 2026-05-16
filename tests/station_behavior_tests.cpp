#include "station/Stations.h"
#include "station/StationFactory.h"

#include <cassert>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>

int main()
{
    StationA stationA;
    StationB stationB;
    StationC stationC;
    StationD stationD;

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

    ProcessResult zeroResultC = stationC.process(0);
    assert(zeroResultC.value == 0);
    assert(zeroResultC.nextStepOffset == 2);

    ProcessResult negativeOddResultC = stationC.process(-1);
    assert(negativeOddResultC.value == -1);
    assert(negativeOddResultC.nextStepOffset == 1);

    ProcessResult negativeEvenResultC = stationC.process(-2);
    assert(negativeEvenResultC.value == -2);
    assert(negativeEvenResultC.nextStepOffset == 2);

    assert(stationD.name() == "D");
    ProcessResult oddResultD = stationD.process(5);
    assert(oddResultD.value == 6);
    assert(oddResultD.nextStepOffset == 1);

    ProcessResult evenResultD = stationD.process(6);
    assert(evenResultD.value == 8);
    assert(evenResultD.nextStepOffset == 1);

    ProcessResult negativeOddResultD = stationD.process(-1);
    assert(negativeOddResultD.value == 0);
    assert(negativeOddResultD.nextStepOffset == 1);

    ProcessResult negativeEvenResultD = stationD.process(-2);
    assert(negativeEvenResultD.value == 0);
    assert(negativeEvenResultD.nextStepOffset == 1);

    bool stationDOddOverflowThrew = false;
    try {
        stationD.process(std::numeric_limits<int>::max());
    } catch (const std::overflow_error& error) {
        stationDOddOverflowThrew = true;
        assert(std::string(error.what()) == "result is outside the supported integer range.");
    }
    assert(stationDOddOverflowThrew);

    bool stationDEvenOverflowThrew = false;
    try {
        stationD.process(std::numeric_limits<int>::max() - 1);
    } catch (const std::overflow_error& error) {
        stationDEvenOverflowThrew = true;
        assert(std::string(error.what()) == "result is outside the supported integer range.");
    }
    assert(stationDEvenOverflowThrew);

    ProcessResult negativeToZeroResultA = stationA.process(-1);
    assert(negativeToZeroResultA.value == 0);
    assert(negativeToZeroResultA.nextStepOffset == 1);

    ProcessResult zeroToNegativeResultB = stationB.process(0);
    assert(zeroToNegativeResultB.value == -1);
    assert(zeroToNegativeResultB.nextStepOffset == 1);

    assert(StationFactory::isSupportedStation("A"));
    assert(StationFactory::isSupportedStation("B"));
    assert(StationFactory::isSupportedStation("C"));
    assert(StationFactory::isSupportedStation("D"));
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

    assert(createdD != nullptr);
    assert(createdD->name() == "D");
    assert(createdEmpty == nullptr);

    return 0;
}
