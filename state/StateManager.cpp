#include "state/StateManager.h"

#include "production/ProductionLine.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {
using json = nlohmann::json;

struct SavedState {
    std::vector<std::string> flow;
    std::vector<int> products;
    std::map<std::string, int> counts;
};

bool hasRequiredFields(const json& data)
{
    return data.contains("flow") && data.contains("products") && data.contains("counts");
}

bool productsFitInInt(const json& products)
{
    if (!products.is_array()) {
        return false;
    }

    for (const json& product : products) {
        if (!product.is_number_integer()) {
            return false;
        }

        const long long value = product.get<long long>();
        if (value < std::numeric_limits<int>::min() || value > std::numeric_limits<int>::max()) {
            return false;
        }
    }

    return true;
}
}

StateManager::StateManager(std::string filePath)
    : filePath(std::move(filePath))
{
}

bool StateManager::stateFileExists() const
{
    std::ifstream input(filePath);
    return input.good();
}

bool StateManager::load(ProductionLine& productionLine) const
{
    try {
        std::ifstream input(filePath);
        if (!input) {
            return false;
        }

        std::ostringstream buffer;
        buffer << input.rdbuf();

        const json data = json::parse(buffer.str());
        if (!hasRequiredFields(data)) {
            return false;
        }

        if (!productsFitInInt(data.at("products"))) {
            return false;
        }

        const SavedState state{
            data.at("flow").get<std::vector<std::string>>(),
            data.at("products").get<std::vector<int>>(),
            data.at("counts").get<std::map<std::string, int>>()
        };

        return productionLine.restoreState(state.flow, state.products, state.counts);
    } catch (const json::exception&) {
        return false;
    } catch (const std::exception&) {
        return false;
    } catch (...) {
        return false;
    }
}

bool StateManager::save(const ProductionLine& productionLine) const
{
    try {
        std::ofstream output(filePath);
        if (!output) {
            return false;
        }

        const json data{
            {"flow", productionLine.getFlow()},
            {"products", productionLine.getProducts()},
            {"counts", productionLine.getStationCounts()}
        };

        output << data.dump(2) << '\n';

        return output.good();
    } catch (const json::exception&) {
        return false;
    } catch (const std::exception&) {
        return false;
    } catch (...) {
        return false;
    }
}
