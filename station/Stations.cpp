#include "station/Stations.h"

std::string StationA::name() const
{
    return "A";
}

ProcessResult StationA::process(int input)
{
    return {input + 1, 1};
}

std::string StationB::name() const
{
    return "B";
}

ProcessResult StationB::process(int input)
{
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
