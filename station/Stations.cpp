#include "station/Stations.h"

#include <limits>
#include <stdexcept>

namespace {
const char* overflowMessage()
{
    return "result is outside the supported integer range.";
}
}

std::string StationA::name() const
{
    return "A";
}

ProcessResult StationA::process(int input)
{
    if (input == std::numeric_limits<int>::max()) {
        throw std::overflow_error(overflowMessage());
    }

    return {input + 1, 1};
}

std::string StationB::name() const
{
    return "B";
}

ProcessResult StationB::process(int input)
{
    if (input == std::numeric_limits<int>::min()) {
        throw std::overflow_error(overflowMessage());
    }

    return {input - 1, 1};
}

std::string StationC::name() const
{
    return "C";
}

ProcessResult StationC::process(int input)
{
    return {input, input % 2 == 0 ? 2 : 1};
}
