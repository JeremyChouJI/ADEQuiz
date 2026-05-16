#ifndef UI_CONSOLEUI_H
#define UI_CONSOLEUI_H

#include "production/ProductionLine.h"
#include "state/StateManager.h"

#include <iosfwd>
#include <string>

class ConsoleUI {
public:
    ConsoleUI(
        ProductionLine& productionLine,
        std::istream& input,
        std::ostream& output,
        const std::string& saveFilePath = "state.json");

    void run();

private:
    void showMenu() const;
    bool handleChoice(const std::string& choice);
    void setProcessingFlow();
    void inputRawMaterial();
    void showCurrentFlow() const;
    void showProducts() const;
    void showStationCounts() const;
    void resetState();

    ProductionLine& productionLine;
    std::istream& input;
    std::ostream& output;
    StateManager stateManager;
};

#endif
