#include "state/StateManager.h"

#include "production/ProductionLine.h"

#include <cstddef>
#include <fstream>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace {
std::string escapeJsonString(const std::string& text)
{
    std::string escaped;
    for (const char ch : text) {
        if (ch == '"') {
            escaped += "\\\"";
        } else if (ch == '\\') {
            escaped += "\\\\";
        } else if (ch == '\n') {
            escaped += "\\n";
        } else if (ch == '\r') {
            escaped += "\\r";
        } else if (ch == '\t') {
            escaped += "\\t";
        } else {
            escaped += ch;
        }
    }

    return escaped;
}

void writeStringArray(std::ostream& output, const std::vector<std::string>& values)
{
    output << '[';
    for (std::size_t index = 0; index < values.size(); ++index) {
        if (index > 0) {
            output << ", ";
        }
        output << '"' << escapeJsonString(values[index]) << '"';
    }
    output << ']';
}

void writeIntArray(std::ostream& output, const std::vector<int>& values)
{
    output << '[';
    for (std::size_t index = 0; index < values.size(); ++index) {
        if (index > 0) {
            output << ", ";
        }
        output << values[index];
    }
    output << ']';
}
}

StateManager::StateManager(std::string filePath)
    : filePath(std::move(filePath))
{
}

bool StateManager::save(const ProductionLine& productionLine) const
{
    try {
        std::ofstream output(filePath);
        if (!output) {
            return false;
        }

        output << "{\n";
        output << "  \"flow\": ";
        writeStringArray(output, productionLine.getFlow());
        output << ",\n";

        output << "  \"products\": ";
        writeIntArray(output, productionLine.getProducts());
        output << ",\n";

        output << "  \"counts\": {";
        const auto& stationCounts = productionLine.getStationCounts();
        if (!stationCounts.empty()) {
            output << '\n';
        }

        std::size_t written = 0;
        for (const auto& stationCount : stationCounts) {
            output << "    \"" << escapeJsonString(stationCount.first) << "\": " << stationCount.second;
            ++written;
            output << (written < stationCounts.size() ? ",\n" : "\n");
        }

        output << "  }\n";
        output << "}\n";

        return output.good();
    } catch (...) {
        return false;
    }
}
