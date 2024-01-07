#include "../include/Vehicle.h"

Vehicle::Vehicle(const std::string& id, const std::string& type) : id(id), type(type) {}

std::string Vehicle::getId() const {
    return id;
}

std::string Vehicle::getType() const {
    return type;
}
