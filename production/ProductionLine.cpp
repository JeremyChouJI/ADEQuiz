#include "production/ProductionLine.h"
#include "flow/FlowParser.h"
#include "station/StationFactory.h"

#include <stdexcept>

bool ProductionLine::setFlow(const std::string& flowText)
{
    const FlowParseResult result = FlowParser::parse(flowText);

    if (!result.success) {
        return false;
    }

    activeRoute = result.stationIds;
    return true;
}

bool ProductionLine::hasFlow() const
{
    return !activeRoute.empty();
}

int ProductionLine::processMaterial(int input)
{
    if (!hasFlow()) {
        throw std::logic_error("No processing flow configured. Please configure a flow first.");
    }

    int currentValue = input;
    std::size_t currentStep = 0;

    while (currentStep < activeRoute.size()) {
        const std::unique_ptr<ProcessingStation> station = StationFactory::createStation(activeRoute[currentStep]);

        if (station == nullptr) {
            throw std::logic_error("Configured processing flow contains an unknown station.");
        }

        const std::string stationName = station->name();
        const ProcessResult result = station->process(currentValue);
        ++stationCounts[stationName];
        currentValue = result.value;
        currentStep += static_cast<std::size_t>(result.nextStepOffset);
    }

    products.push_back(currentValue);
    return currentValue;
}

const std::vector<int>& ProductionLine::getProducts() const
{
    return products;
}

const std::map<std::string, int>& ProductionLine::getStationCounts() const
{
    return stationCounts;
}
