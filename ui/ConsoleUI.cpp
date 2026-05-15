#include "ui/ConsoleUI.h"

#include <cstddef>
#include <istream>
#include <ostream>
#include <stdexcept>

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
    if (choice == "1") {
        setProcessingFlow();
    } else if (choice == "2") {
        inputRawMaterial();
    } else if (choice == "3") {
        showProducts();
    } else if (choice == "4") {
        showStationCounts();
    } else if (choice == "5") {
        return false;
    } else {
        output << "Invalid choice.\n";
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
        output << "Failed to set processing flow.\n";
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
        std::size_t parsedCharacters = 0;
        const int material = std::stoi(rawInput, &parsedCharacters);

        if (parsedCharacters != rawInput.size()) {
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
