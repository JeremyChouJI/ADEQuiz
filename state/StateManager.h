#ifndef STATE_STATEMANAGER_H
#define STATE_STATEMANAGER_H

#include <string>

class ProductionLine;

class StateManager {
public:
    explicit StateManager(std::string filePath = "state.json");

    bool stateFileExists() const;
    bool load(ProductionLine& productionLine) const;
    bool save(const ProductionLine& productionLine) const;

private:
    std::string filePath;
};

#endif
