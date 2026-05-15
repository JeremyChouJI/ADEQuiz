#include "ui/ConsoleUI.h"

#include "production/ProductionLine.h"

#include <cassert>
#include <sstream>
#include <string>

namespace {
bool contains(const std::string& text, const std::string& expected)
{
    return text.find(expected) != std::string::npos;
}
}

int main()
{
    {
        ProductionLine line;
        std::istringstream input("1\nA->B->C->B->A\n2\n2\n3\n4\n5\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "Manufacturing Station Console"));
        assert(contains(text, "Processing flow set successfully."));
        assert(contains(text, "Final product: 3"));
        assert(contains(text, "Products:\n1. 3\n\nTotal: 1 product"));
        assert(contains(text, "Station A processed 2 times"));
        assert(contains(text, "Station B processed 1 time"));
        assert(contains(text, "Station C processed 1 time"));
    }

    {
        ProductionLine line;
        std::istringstream input("3\n4\n5\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "No products have been produced yet."));
        assert(contains(text, "No station has processed material yet."));
    }

    return 0;
}
