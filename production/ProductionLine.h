#ifndef PRODUCTION_PRODUCTIONLINE_H
#define PRODUCTION_PRODUCTIONLINE_H

#include <string>
#include <vector>

class ProductionLine {
public:
    bool setFlow(const std::string& flowText);
    bool hasFlow() const;
    int processMaterial(int input) const;

private:
    std::vector<std::string> activeRoute;
};

#endif
