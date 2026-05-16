#include "ui/ConsoleUI.h"

#include "production/ProductionLine.h"

#include <nlohmann/json.hpp>

#include <cassert>
#include <cstdio>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

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

nlohmann::json readJsonFile(const std::string& path)
{
    return nlohmann::json::parse(readFile(path));
}

bool fileExists(const std::string& path)
{
    std::ifstream input(path);
    return input.good();
}
}

int main()
{
    std::remove("state.json");

    {
        const std::string savePath = "adequiz_console_ui_test_state.json";
        std::remove(savePath.c_str());

        ProductionLine line;
        std::istringstream input("1\nA->B->C->B->A\n2\n2\n3\n4\n5\n7\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output, savePath);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "Manufacturing Station Console"));
        assert(contains(text, "Processing flow set successfully."));
        assert(contains(text, "Final product: 3"));
        assert(contains(text, "Current processing flow: A->B->C->B->A"));
        assert(contains(text, "Products:\n1. 3\n\nTotal: 1 product"));
        assert(contains(text, "Station A processed 2 times"));
        assert(contains(text, "Station B processed 1 time"));
        assert(contains(text, "Station C processed 1 time"));

        const nlohmann::json savedJson = readJsonFile(savePath);
        assert((savedJson.at("flow").get<std::vector<std::string>>() == std::vector<std::string>{"A", "B", "C", "B", "A"}));
        assert((savedJson.at("products").get<std::vector<int>>() == std::vector<int>{3}));
        assert(savedJson.at("counts").at("A").get<int>() == 2);

        std::remove(savePath.c_str());
    }

    {
        const std::string savePath = "C:\\tmp\\adequiz_missing_directory\\state.json";
        ProductionLine line;
        std::istringstream input("7\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output, savePath);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "Warning: Could not save application state."));
    }

    {
        const std::string savePath = "adequiz_console_ui_restore_state.json";
        std::remove(savePath.c_str());

        {
            std::ofstream output(savePath);
            output << "{\n";
            output << "  \"flow\": [\"A\", \"B\", \"C\", \"B\", \"A\"],\n";
            output << "  \"products\": [3],\n";
            output << "  \"counts\": {\"A\": 2, \"B\": 1, \"C\": 1}\n";
            output << "}\n";
        }

        ProductionLine line;
        std::istringstream input("4\n5\n2\n2\n7\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output, savePath);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "Products:\n1. 3\n\nTotal: 1 product"));
        assert(contains(text, "Station A processed 2 times"));
        assert(contains(text, "Final product: 3"));
        assert((line.getProducts() == std::vector<int>{3, 3}));

        std::remove(savePath.c_str());
    }

    {
        const std::string savePath = "adequiz_console_ui_auto_save_state.json";
        std::remove(savePath.c_str());

        ProductionLine line;
        std::istringstream input("1\nA->B->C->B->A\n2\n2\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output, savePath);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "Final product: 3"));

        const nlohmann::json savedJson = readJsonFile(savePath);
        assert((savedJson.at("flow").get<std::vector<std::string>>() == std::vector<std::string>{"A", "B", "C", "B", "A"}));
        assert((savedJson.at("products").get<std::vector<int>>() == std::vector<int>{3}));
        assert((savedJson.at("counts").get<std::map<std::string, int>>() == std::map<std::string, int>{{"A", 2}, {"B", 1}, {"C", 1}}));

        std::remove(savePath.c_str());
    }

    {
        const std::string savePath = "adequiz_console_ui_no_auto_save_invalid_input.json";
        std::remove(savePath.c_str());

        ProductionLine line;
        std::istringstream input("1\nA->B\n2\nabc\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output, savePath);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "Invalid input. Please enter an integer."));
        assert(!fileExists(savePath));
    }

    {
        const std::string savePath = "adequiz_console_ui_no_auto_save_without_flow.json";
        std::remove(savePath.c_str());

        ProductionLine line;
        std::istringstream input("2\n1\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output, savePath);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "No processing flow configured. Please configure a flow first."));
        assert(!fileExists(savePath));
    }

    {
        const std::string savePath = "adequiz_console_ui_invalid_state.json";
        std::remove(savePath.c_str());

        {
            std::ofstream output(savePath);
            output << "{ invalid json";
        }

        ProductionLine line;
        std::istringstream input("4\n7\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output, savePath);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "Warning: Save file is invalid. Starting with empty state."));
        assert(contains(text, "No products have been produced yet."));

        std::remove(savePath.c_str());
    }

    {
        ProductionLine line;
        std::istringstream input("3\n4\n5\n7\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "No processing flow configured."));
        assert(contains(text, "No products have been produced yet."));
        assert(contains(text, "No station has processed material yet."));
    }

    {
        ProductionLine line;
        std::istringstream input("abc\n12abc\n3.14\n1 2\n0\n8\n7\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "Invalid choice. Please choose a number from 1 to 7."));
    }

    {
        ProductionLine line;
        std::istringstream input(" 7 \n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output);

        ui.run();

        const std::string text = output.str();
        assert(!contains(text, "Invalid choice. Please choose a number from 1 to 7."));
    }

    {
        ProductionLine line;
        std::istringstream input("2\nabc\n2\n12abc\n2\n3.14\n2\n1 2\n7\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "Invalid input. Please enter an integer."));
    }

    {
        ProductionLine line;
        std::istringstream input("2\n1\n7\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "No processing flow configured. Please configure a flow first."));
    }

    {
        const std::string savePath = "adequiz_console_ui_whitespace_raw_state.json";
        std::remove(savePath.c_str());

        ProductionLine line;
        std::istringstream input("1\nC\n2\n 42 \n7\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output, savePath);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "Processing flow set successfully."));
        assert(contains(text, "Final product: 42"));
        assert((line.getFlow() == std::vector<std::string>{"C"}));
        assert((line.getProducts() == std::vector<int>{42}));

        std::remove(savePath.c_str());
    }

    {
        const std::string savePath = "adequiz_console_ui_negative_raw_state.json";
        std::remove(savePath.c_str());

        ProductionLine line;
        std::istringstream input("1\nC\n2\n-1\n7\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output, savePath);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "Final product: -1"));
        assert((line.getProducts() == std::vector<int>{-1}));

        std::remove(savePath.c_str());
    }

    {
        const std::string savePath = "adequiz_console_ui_max_raw_state.json";
        std::remove(savePath.c_str());

        ProductionLine line;
        std::istringstream input("1\nC\n2\n2147483647\n7\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output, savePath);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "Final product: 2147483647"));
        assert((line.getProducts() == std::vector<int>{2147483647}));

        std::remove(savePath.c_str());
    }

    {
        const std::string savePath = "adequiz_console_ui_min_raw_state.json";
        std::remove(savePath.c_str());

        ProductionLine line;
        std::istringstream input("1\nC\n2\n-2147483648\n7\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output, savePath);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "Final product: -2147483648"));
        assert((line.getProducts() == std::vector<int>{-2147483647 - 1}));

        std::remove(savePath.c_str());
    }

    {
        const std::string savePath = "adequiz_console_ui_out_of_range_raw_state.json";
        std::remove(savePath.c_str());

        ProductionLine line;
        std::istringstream input("1\nC\n2\n2147483648\n7\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output, savePath);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "Invalid input. Please enter an integer."));
        assert(line.getProducts().empty());

        const nlohmann::json savedJson = readJsonFile(savePath);
        assert((savedJson.at("products").get<std::vector<int>>() == std::vector<int>{}));
        assert((savedJson.at("counts").get<std::map<std::string, int>>() == std::map<std::string, int>{}));

        std::remove(savePath.c_str());
    }

    {
        const std::string savePath = "adequiz_console_ui_station_a_overflow_state.json";
        std::remove(savePath.c_str());

        ProductionLine line;
        std::istringstream input("1\nA\n2\n2147483647\n7\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output, savePath);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "Processing failed: result is outside the supported integer range."));
        assert(!contains(text, "Final product:"));
        assert(line.getProducts().empty());
        assert(line.getStationCounts().empty());

        const nlohmann::json savedJson = readJsonFile(savePath);
        assert((savedJson.at("products").get<std::vector<int>>() == std::vector<int>{}));
        assert((savedJson.at("counts").get<std::map<std::string, int>>() == std::map<std::string, int>{}));

        std::remove(savePath.c_str());
    }

    {
        const std::string savePath = "adequiz_console_ui_station_b_overflow_state.json";
        std::remove(savePath.c_str());

        ProductionLine line;
        std::istringstream input("1\nB\n2\n-2147483648\n7\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output, savePath);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "Processing failed: result is outside the supported integer range."));
        assert(!contains(text, "Final product:"));
        assert(line.getProducts().empty());
        assert(line.getStationCounts().empty());

        const nlohmann::json savedJson = readJsonFile(savePath);
        assert((savedJson.at("products").get<std::vector<int>>() == std::vector<int>{}));
        assert((savedJson.at("counts").get<std::map<std::string, int>>() == std::map<std::string, int>{}));

        std::remove(savePath.c_str());
    }

    {
        ProductionLine line;
        std::istringstream input("1\nA->\n1\nA->E->B\n7\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "Invalid flow format. Please use format like A->B->C."));
        assert(contains(text, "Unknown station: E"));
    }

    {
        ProductionLine line;
        std::istringstream input("1\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "Failed to read processing flow."));
        assert(!line.hasFlow());
    }

    {
        ProductionLine line;
        std::istringstream input("1\nC\n2\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "Failed to read raw material."));
        assert(line.getProducts().empty());
    }

    {
        const std::string savePath = "adequiz_console_ui_reset_state.json";
        std::remove(savePath.c_str());

        ProductionLine line;
        std::istringstream input("1\nA->B->C->B->A\n2\n2\n6\nRESET\n3\n4\n5\n7\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output, savePath);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "Type RESET to confirm clearing all saved state:"));
        assert(contains(text, "Application state has been reset."));
        assert(contains(text, "No processing flow configured."));
        assert(contains(text, "No products have been produced yet."));
        assert(contains(text, "No station has processed material yet."));
        assert(line.getFlow().empty());
        assert(line.getProducts().empty());
        assert(line.getStationCounts().empty());

        const nlohmann::json savedJson = readJsonFile(savePath);
        assert((savedJson.at("flow").get<std::vector<std::string>>() == std::vector<std::string>{}));
        assert((savedJson.at("products").get<std::vector<int>>() == std::vector<int>{}));
        assert((savedJson.at("counts").get<std::map<std::string, int>>() == std::map<std::string, int>{}));

        std::remove(savePath.c_str());
    }

    {
        const std::string savePath = "adequiz_console_ui_reset_cancel_state.json";
        std::remove(savePath.c_str());

        ProductionLine line;
        std::istringstream input("1\nA->B\n2\n5\n6\nno\n3\n4\n5\n7\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output, savePath);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "State reset canceled."));
        assert(contains(text, "Current processing flow: A->B"));
        assert(contains(text, "Products:\n1. 5\n\nTotal: 1 product"));
        assert(contains(text, "Station A processed 1 time"));
        assert(contains(text, "Station B processed 1 time"));
        assert((line.getFlow() == std::vector<std::string>{"A", "B"}));
        assert((line.getProducts() == std::vector<int>{5}));

        std::remove(savePath.c_str());
    }

    {
        const std::string savePath = "adequiz_console_ui_reset_whitespace_cancel_state.json";
        std::remove(savePath.c_str());

        ProductionLine line;
        std::istringstream input("1\nA->B\n2\n5\n6\n RESET \n3\n4\n5\n7\n");
        std::ostringstream output;
        ConsoleUI ui(line, input, output, savePath);

        ui.run();

        const std::string text = output.str();
        assert(contains(text, "State reset canceled."));
        assert(contains(text, "Current processing flow: A->B"));
        assert(contains(text, "Products:\n1. 5\n\nTotal: 1 product"));
        assert(contains(text, "Station A processed 1 time"));
        assert(contains(text, "Station B processed 1 time"));
        assert((line.getFlow() == std::vector<std::string>{"A", "B"}));
        assert((line.getProducts() == std::vector<int>{5}));

        std::remove(savePath.c_str());
    }

    std::remove("state.json");
    return 0;
}
