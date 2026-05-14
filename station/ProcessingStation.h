#ifndef STATION_PROCESSINGSTATION_H
#define STATION_PROCESSINGSTATION_H

#include <string>

struct ProcessResult {
    int value;
    int nextStepOffset;
};

class ProcessingStation {
public:
    virtual ~ProcessingStation() = default;

    virtual std::string name() const = 0;
    virtual ProcessResult process(int input) = 0;
};

#endif
