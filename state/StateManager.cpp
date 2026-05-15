#include "state/StateManager.h"

#include "production/ProductionLine.h"

#include <cstddef>
#include <cctype>
#include <fstream>
#include <limits>
#include <map>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {
struct SavedState {
    std::vector<std::string> flow;
    std::vector<int> products;
    std::map<std::string, int> counts;
};

class JsonReader {
public:
    explicit JsonReader(std::string text)
        : text(std::move(text))
    {
    }

    bool parseSavedState(SavedState& state)
    {
        bool hasFlow = false;
        bool hasProducts = false;
        bool hasCounts = false;

        if (!consume('{')) {
            return false;
        }

        if (consume('}')) {
            return false;
        }

        while (true) {
            std::string key;
            if (!parseString(key) || !consume(':')) {
                return false;
            }

            if (key == "flow") {
                if (hasFlow || !parseStringArray(state.flow)) {
                    return false;
                }
                hasFlow = true;
            } else if (key == "products") {
                if (hasProducts || !parseIntArray(state.products)) {
                    return false;
                }
                hasProducts = true;
            } else if (key == "counts") {
                if (hasCounts || !parseCountsObject(state.counts)) {
                    return false;
                }
                hasCounts = true;
            } else if (!skipValue()) {
                return false;
            }

            if (consume('}')) {
                break;
            }

            if (!consume(',')) {
                return false;
            }
        }

        skipWhitespace();
        return atEnd() && hasFlow && hasProducts && hasCounts;
    }

private:
    bool atEnd() const
    {
        return position >= text.size();
    }

    char peek() const
    {
        return atEnd() ? '\0' : text[position];
    }

    void skipWhitespace()
    {
        while (!atEnd() && std::isspace(static_cast<unsigned char>(text[position]))) {
            ++position;
        }
    }

    bool consume(char expected)
    {
        const std::size_t originalPosition = position;
        skipWhitespace();

        if (peek() != expected) {
            position = originalPosition;
            return false;
        }

        ++position;
        return true;
    }

    bool parseString(std::string& value)
    {
        if (!consume('"')) {
            return false;
        }

        value.clear();
        while (!atEnd()) {
            const char ch = text[position++];

            if (ch == '"') {
                return true;
            }

            if (static_cast<unsigned char>(ch) < 0x20) {
                return false;
            }

            if (ch != '\\') {
                value += ch;
                continue;
            }

            if (atEnd()) {
                return false;
            }

            const char escaped = text[position++];
            if (escaped == '"' || escaped == '\\' || escaped == '/') {
                value += escaped;
            } else if (escaped == 'b') {
                value += '\b';
            } else if (escaped == 'f') {
                value += '\f';
            } else if (escaped == 'n') {
                value += '\n';
            } else if (escaped == 'r') {
                value += '\r';
            } else if (escaped == 't') {
                value += '\t';
            } else {
                return false;
            }
        }

        return false;
    }

    bool parseInt(int& value)
    {
        skipWhitespace();
        const std::size_t start = position;

        if (peek() == '-') {
            ++position;
        }

        if (!std::isdigit(static_cast<unsigned char>(peek()))) {
            return false;
        }

        if (peek() == '0') {
            ++position;
            if (std::isdigit(static_cast<unsigned char>(peek()))) {
                return false;
            }
        } else {
            while (std::isdigit(static_cast<unsigned char>(peek()))) {
                ++position;
            }
        }

        try {
            const long long parsed = std::stoll(text.substr(start, position - start));
            if (parsed < std::numeric_limits<int>::min() || parsed > std::numeric_limits<int>::max()) {
                return false;
            }

            value = static_cast<int>(parsed);
            return true;
        } catch (...) {
            return false;
        }
    }

    bool parseStringArray(std::vector<std::string>& values)
    {
        std::vector<std::string> parsedValues;
        if (!consume('[')) {
            return false;
        }

        if (consume(']')) {
            values = parsedValues;
            return true;
        }

        while (true) {
            std::string value;
            if (!parseString(value)) {
                return false;
            }

            parsedValues.push_back(value);

            if (consume(']')) {
                values = parsedValues;
                return true;
            }

            if (!consume(',')) {
                return false;
            }
        }
    }

    bool parseIntArray(std::vector<int>& values)
    {
        std::vector<int> parsedValues;
        if (!consume('[')) {
            return false;
        }

        if (consume(']')) {
            values = parsedValues;
            return true;
        }

        while (true) {
            int value = 0;
            if (!parseInt(value)) {
                return false;
            }

            parsedValues.push_back(value);

            if (consume(']')) {
                values = parsedValues;
                return true;
            }

            if (!consume(',')) {
                return false;
            }
        }
    }

    bool parseCountsObject(std::map<std::string, int>& counts)
    {
        std::map<std::string, int> parsedCounts;
        if (!consume('{')) {
            return false;
        }

        if (consume('}')) {
            counts = parsedCounts;
            return true;
        }

        while (true) {
            std::string stationId;
            int count = 0;
            if (!parseString(stationId) || !consume(':') || !parseInt(count)) {
                return false;
            }

            if (!parsedCounts.emplace(stationId, count).second) {
                return false;
            }

            if (consume('}')) {
                counts = parsedCounts;
                return true;
            }

            if (!consume(',')) {
                return false;
            }
        }
    }

    bool skipValue()
    {
        skipWhitespace();

        if (peek() == '"') {
            std::string ignored;
            return parseString(ignored);
        }
        if (peek() == '{') {
            return skipObject();
        }
        if (peek() == '[') {
            return skipArray();
        }
        if (peek() == '-' || std::isdigit(static_cast<unsigned char>(peek()))) {
            int ignored = 0;
            return parseInt(ignored);
        }

        return skipLiteral("true") || skipLiteral("false") || skipLiteral("null");
    }

    bool skipObject()
    {
        if (!consume('{')) {
            return false;
        }

        if (consume('}')) {
            return true;
        }

        while (true) {
            std::string key;
            if (!parseString(key) || !consume(':') || !skipValue()) {
                return false;
            }

            if (consume('}')) {
                return true;
            }

            if (!consume(',')) {
                return false;
            }
        }
    }

    bool skipArray()
    {
        if (!consume('[')) {
            return false;
        }

        if (consume(']')) {
            return true;
        }

        while (true) {
            if (!skipValue()) {
                return false;
            }

            if (consume(']')) {
                return true;
            }

            if (!consume(',')) {
                return false;
            }
        }
    }

    bool skipLiteral(const std::string& literal)
    {
        skipWhitespace();
        if (text.compare(position, literal.size(), literal) != 0) {
            return false;
        }

        position += literal.size();
        return true;
    }

    std::string text;
    std::size_t position = 0;
};

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

bool StateManager::stateFileExists() const
{
    std::ifstream input(filePath);
    return input.good();
}

bool StateManager::load(ProductionLine& productionLine) const
{
    try {
        std::ifstream input(filePath);
        if (!input) {
            return false;
        }

        std::ostringstream buffer;
        buffer << input.rdbuf();

        SavedState state;
        JsonReader reader(buffer.str());
        if (!reader.parseSavedState(state)) {
            return false;
        }

        return productionLine.restoreState(state.flow, state.products, state.counts);
    } catch (...) {
        return false;
    }
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
