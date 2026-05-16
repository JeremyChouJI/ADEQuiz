#ifndef STATION_STATIONS_H
#define STATION_STATIONS_H

#include "station/ProcessingStation.h"

class StationA : public ProcessingStation {
public:
    std::string name() const override;
    ProcessResult process(int input) override;
};

class StationB : public ProcessingStation {
public:
    std::string name() const override;
    ProcessResult process(int input) override;
};

class StationC : public ProcessingStation {
public:
    std::string name() const override;
    ProcessResult process(int input) override;
};

class StationD : public ProcessingStation {
public:
    std::string name() const override;
    ProcessResult process(int input) override;
};

#endif
