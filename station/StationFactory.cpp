#include "station/StationFactory.h"

#include "station/Stations.h"

#include <functional>
#include <map>

namespace {
using StationCreator = std::function<std::unique_ptr<ProcessingStation>()>;

const std::map<std::string, StationCreator>& stationCreators()
{
    static const std::map<std::string, StationCreator> creators = {
        {"A", []() { return std::make_unique<StationA>(); }},
        {"B", []() { return std::make_unique<StationB>(); }},
        {"C", []() { return std::make_unique<StationC>(); }},
    };

    return creators;
}
}

/*根據傳入的識別ID，建立對應的 Station Object*/
std::unique_ptr<ProcessingStation> StationFactory::createStation(const std::string& stationId)
{
    const auto& creators = stationCreators();
    const auto found = creators.find(stationId);

    if (found == creators.end()) {
        return nullptr;
    }

    return found->second();
}

bool StationFactory::isSupportedStation(const std::string& stationId)
{
    const auto& creators = stationCreators();
    return creators.find(stationId) != creators.end();
}
