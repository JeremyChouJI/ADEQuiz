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
    assertSuccess("A", {"A"});
    assertSuccess("A->B", {"A", "B"});
    assertSuccess("C->B->A", {"C", "B", "A"});
    assertSuccess("A->D->B", {"A", "D", "B"});
    assertSuccess("A->B->C->B->A", {"A", "B", "C", "B", "A"});
    assertSuccess(" A -> B ", {"A", "B"});
    assertSuccess("A-> B ->C", {"A", "B", "C"});

    assertFormatError("");
    assertFormatError("   ");
    assertFormatError("->A");
    assertFormatError("A->");
    assertFormatError("A -> ");
    assertFormatError("A->B->   ");
    assertFormatError("A->->B");
    assertFormatError("A--B");
    assertFormatError("A=>B");
    assertFormatError("A - > B");

    assertUnknownStation("A->E->B", "E");
    assertUnknownStation("a", "a");
    assertUnknownStation("AA", "AA");
    assertUnknownStation("A1", "A1");

    return 0;
}
