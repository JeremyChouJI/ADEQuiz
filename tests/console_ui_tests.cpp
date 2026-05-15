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

    {
        ProductionLine line;
        std::istringstream input("abc\n12abc\n3.14\n1 2\n0\n6\n5\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "Invalid choice. Please choose a number from 1 to 5."));
    }

    {
        ProductionLine line;
        std::istringstream input("2\nabc\n2\n12abc\n2\n3.14\n2\n1 2\n5\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "Invalid input. Please enter an integer."));
    }

    {
        ProductionLine line;
        std::istringstream input("2\n1\n5\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "No processing flow configured. Please configure a flow first."));
    }

    {
        ProductionLine line;
        std::istringstream input("1\nA->\n1\nA->D->B\n5\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "Invalid flow format. Please use format like A->B->C."));
        assert(contains(text, "Unknown station: D"));
    }

    return 0;
}
