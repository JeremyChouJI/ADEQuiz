#include "ui/ConsoleUI.h"

#include "production/ProductionLine.h"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

namespace {
bool contains(const std::string& text, const std::string& expected)
{
    return text.find(expected) != std::string::npos;
}

std::string readFile(const std::string& path)
{
    std::ifstream input(path);
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}
}

int main()
{
    std::remove("state.json");

    {
        const std::string savePath = "adequiz_console_ui_test_state.json";
        std::remove(savePath.c_str());

        ProductionLine line;
        std::istringstream input("1\nA->B->C->B->A\n2\n2\n3\n4\n5\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output, savePath);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "Manufacturing Station Console"));
        assert(contains(text, "Processing flow set successfully."));
        assert(contains(text, "Final product: 3"));
        assert(contains(text, "Products:\n1. 3\n\nTotal: 1 product"));
        assert(contains(text, "Station A processed 2 times"));
        assert(contains(text, "Station B processed 1 time"));
        assert(contains(text, "Station C processed 1 time"));

        const std::string json = readFile(savePath);
        assert(contains(json, "\"flow\": [\"A\", \"B\", \"C\", \"B\", \"A\"]"));
        assert(contains(json, "\"products\": [3]"));
        assert(contains(json, "\"A\": 2"));

        std::remove(savePath.c_str());
    }

    {
        const std::string savePath = "C:\\tmp\\adequiz_missing_directory\\state.json";
        ProductionLine line;
        std::istringstream input("5\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output, savePath);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "Warning: Could not save application state."));
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

    std::remove("state.json");
    return 0;
}
