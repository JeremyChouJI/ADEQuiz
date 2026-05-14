#ifndef FLOW_FLOWPARSER_H
#define FLOW_FLOWPARSER_H

#include <string>
#include <vector>

/*struct FlowParseResult: 1.是否成功 2.識別ID向量 3.錯誤訊息*/
struct FlowParseResult {
    bool success;
    std::vector<std::string> stationIds;
    std::string errorMessage;
};

class FlowParser {
public:
    static FlowParseResult parse(const std::string& flowText);
};

#endif
