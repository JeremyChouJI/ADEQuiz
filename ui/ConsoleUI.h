#ifndef UI_CONSOLEUI_H
#define UI_CONSOLEUI_H

#include "production/ProductionLine.h"

#include <iosfwd>
#include <string>

class ConsoleUI {
public:
    ConsoleUI(ProductionLine& productionLine, std::istream& input, std::ostream& output);

    void run();

private:
    void showMenu() const;
    bool handleChoice(const std::string& choice);
    void setProcessingFlow();
    void inputRawMaterial();
    void showProducts() const;
    void showStationCounts() const;

    ProductionLine& productionLine;
    std::istream& input;
    std::ostream& output;
};

#endif
