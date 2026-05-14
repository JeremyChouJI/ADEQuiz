#include "flow/FlowParser.h"
#include "station/StationFactory.h"

#include <algorithm>
#include <cctype>

namespace {
const char* invalidFlowMessage()
{
    return "Invalid flow format. Please use format like A->B->C.";
}

/*移除字串前後的空白，讓使用者輸入時可以包含多餘空格*/
std::string trim(const std::string& text)
{
    const auto first = std::find_if_not(text.begin(), text.end(), [](unsigned char ch) {
        return std::isspace(ch);
    });

    if (first == text.end()) {
        return "";
    }

    const auto last = std::find_if_not(text.rbegin(), text.rend(), [](unsigned char ch) {
        return std::isspace(ch);
    }).base();

    return std::string(first, last);
}

bool isValidStationIdFormat(const std::string& stationId)
{
    return !stationId.empty()
        && std::all_of(stationId.begin(), stationId.end(), [](unsigned char ch) {
               return std::isalnum(ch);
           });
}

FlowParseResult formatError()
{
    return {false, {}, invalidFlowMessage()};
}

FlowParseResult unknownStationError(const std::string& stationId)
{
    return {false, {}, "Unknown station: " + stationId};
}
}

/*分析輸入的流程文字(e.g. A->B->C)，並檢查格式與站點是否有效，最終輸出成["A", "B", "C"]*/
FlowParseResult FlowParser::parse(const std::string& flowText)
{
    const std::string trimmedFlow = trim(flowText);

    if (trimmedFlow.empty()) {
        return formatError();
    }

    std::vector<std::string> stationIds;
    std::size_t start = 0;

    while (start <= trimmedFlow.size()) {
        const std::size_t delimiter = trimmedFlow.find("->", start);
        const std::size_t end = delimiter == std::string::npos ? trimmedFlow.size() : delimiter;
        const std::string stationId = trim(trimmedFlow.substr(start, end - start));

        if (!isValidStationIdFormat(stationId)) {
            return formatError();
        }

        if (!StationFactory::isSupportedStation(stationId)) {
            return unknownStationError(stationId);
        }

        stationIds.push_back(stationId);

        if (delimiter == std::string::npos) {
            break;
        }

        start = delimiter + 2;
    }

    return {true, stationIds, ""};
}
