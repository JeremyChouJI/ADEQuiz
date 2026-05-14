#include "station/Stations.h"

int main()
{
    StationA stationA;
    StationB stationB;
    StationC stationC;

    stationA.process(1);
    stationB.process(1);
    stationC.process(1);
    stationC.process(2);

    return 0;
}
