#include "flow/FlowParser.h"

#include <cassert>
#include <string>
#include <vector>

namespace {
void assertSuccess(const std::string& flowText, const std::vector<std::string>& expectedStationIds)
{
    const FlowParseResult result = FlowParser::parse(flowText);

    assert(result.success);
    assert(result.stationIds == expectedStationIds);
    assert(result.errorMessage.empty());
}

void assertFormatError(const std::string& flowText)
{
    const FlowParseResult result = FlowParser::parse(flowText);

    assert(!result.success);
    assert(result.stationIds.empty());
    assert(result.errorMessage == "Invalid flow format. Please use format like A->B->C.");
}

void assertUnknownStation(const std::string& flowText, const std::string& stationId)
{
    const FlowParseResult result = FlowParser::parse(flowText);

    assert(!result.success);
    assert(result.stationIds.empty());
    assert(result.errorMessage == "Unknown station: " + stationId);
}
}

int main()
{
    assertSuccess("A->B", {"A", "B"});
    assertSuccess("C->B->A", {"C", "B", "A"});
    assertSuccess("A->B->C->B->A", {"A", "B", "C", "B", "A"});

    assertFormatError("");
    assertFormatError("   ");
    assertFormatError("->A");
    assertFormatError("A->");
    assertFormatError("A->->B");
    assertFormatError("A--B");
    assertFormatError("A=>B");

    assertUnknownStation("A->D->B", "D");

    return 0;
}
