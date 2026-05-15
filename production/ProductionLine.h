#ifndef PRODUCTION_PRODUCTIONLINE_H
#define PRODUCTION_PRODUCTIONLINE_H

#include <map>
#include <string>
#include <vector>

class ProductionLine {
public:
    bool setFlow(const std::string& flowText);
    bool hasFlow() const;
    int processMaterial(int input);
    const std::vector<int>& getProducts() const;
    const std::map<std::string, int>& getStationCounts() const;

private:
    std::vector<std::string> activeRoute;
    std::vector<int> products;
    std::map<std::string, int> stationCounts;
};

#endif
