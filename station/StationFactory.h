#ifndef STATION_STATIONFACTORY_H
#define STATION_STATIONFACTORY_H

#include "station/ProcessingStation.h"

#include <memory>
#include <string>

class StationFactory {
public:
    static std::unique_ptr<ProcessingStation> createStation(const std::string& stationId);
    static bool isSupportedStation(const std::string& stationId);
};

#endif
