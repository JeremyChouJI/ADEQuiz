#include "ui/ConsoleUI.h"

#include <cstddef>
#include <algorithm>
#include <cctype>
#include <istream>
#include <ostream>
#include <stdexcept>

namespace {
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

bool parseInteger(const std::string& text, int& value)
{
    const std::string trimmedText = trim(text);

    if (trimmedText.empty()) {
        return false;
    }

    try {
        std::size_t parsedCharacters = 0;
        value = std::stoi(trimmedText, &parsedCharacters);
        return parsedCharacters == trimmedText.size();
    } catch (const std::invalid_argument&) {
        return false;
    } catch (const std::out_of_range&) {
        return false;
    }
}
}

ConsoleUI::ConsoleUI(ProductionLine& productionLine, std::istream& input, std::ostream& output)
    : productionLine(productionLine)
    , input(input)
    , output(output)
{
}

void ConsoleUI::run()
{
    bool running = true;

    while (running) {
        showMenu();

        std::string choice;
        if (!std::getline(input, choice)) {
            break;
        }

        running = handleChoice(choice);
    }
}

void ConsoleUI::showMenu() const
{
    output << "==============================\n";
    output << "Manufacturing Station Console\n";
    output << "==============================\n";
    output << "1. Set processing flow\n";
    output << "2. Input raw material\n";
    output << "3. Show all products\n";
    output << "4. Show station processing counts\n";
    output << "5. Exit and save\n";
    output << "Please choose:\n";
}

bool ConsoleUI::handleChoice(const std::string& choice)
{
    int menuChoice = 0;
    if (!parseInteger(choice, menuChoice) || menuChoice < 1 || menuChoice > 5) {
        output << "Invalid choice. Please choose a number from 1 to 5.\n";
        output << '\n';
        return true;
    }

    if (menuChoice == 1) {
        setProcessingFlow();
    } else if (menuChoice == 2) {
        inputRawMaterial();
    } else if (menuChoice == 3) {
        showProducts();
    } else if (menuChoice == 4) {
        showStationCounts();
    } else if (menuChoice == 5) {
        return false;
    }

    output << '\n';
    return true;
}

void ConsoleUI::setProcessingFlow()
{
    output << "Enter processing flow:\n";

    std::string flowText;
    if (!std::getline(input, flowText)) {
        output << "Failed to read processing flow.\n";
        return;
    }

    if (productionLine.setFlow(flowText)) {
        output << "Processing flow set successfully.\n";
    } else {
        output << productionLine.getLastFlowError() << '\n';
    }
}

void ConsoleUI::inputRawMaterial()
{
    output << "Enter raw material:\n";

    std::string rawInput;
    if (!std::getline(input, rawInput)) {
        output << "Failed to read raw material.\n";
        return;
    }

    try {
        int material = 0;
        if (!parseInteger(rawInput, material)) {
            output << "Invalid input. Please enter an integer.\n";
            return;
        }

        const int finalProduct = productionLine.processMaterial(material);
        output << "Final product: " << finalProduct << '\n';
    } catch (const std::invalid_argument&) {
        output << "Invalid input. Please enter an integer.\n";
    } catch (const std::out_of_range&) {
        output << "Invalid input. Please enter an integer.\n";
    } catch (const std::logic_error& error) {
        output << error.what() << '\n';
    }
}

void ConsoleUI::showProducts() const
{
    const auto& products = productionLine.getProducts();

    if (products.empty()) {
        output << "No products have been produced yet.\n";
        return;
    }

    output << "Products:\n";
    for (std::size_t index = 0; index < products.size(); ++index) {
        output << index + 1 << ". " << products[index] << '\n';
    }

    output << "\nTotal: " << products.size() << ' ';
    output << (products.size() == 1 ? "product" : "products") << '\n';
}

void ConsoleUI::showStationCounts() const
{
    const auto& stationCounts = productionLine.getStationCounts();

    if (stationCounts.empty()) {
        output << "No station has processed material yet.\n";
        return;
    }

    for (const auto& stationCount : stationCounts) {
        output << "Station " << stationCount.first << " processed " << stationCount.second << ' ';
        output << (stationCount.second == 1 ? "time" : "times") << '\n';
    }
}
