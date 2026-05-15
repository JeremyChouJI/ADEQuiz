#include "production/ProductionLine.h"
#include "ui/ConsoleUI.h"

#include <iostream>

int main()
{
    ProductionLine productionLine;
    ConsoleUI ui(productionLine, std::cin, std::cout);
    ui.run();

    return 0;
}
