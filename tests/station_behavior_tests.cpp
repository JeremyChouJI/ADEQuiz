#include "station/Stations.h"

#include <cassert>
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

    return 0;
}
