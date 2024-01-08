#include "../include/Vehicle.h"
#include <stdexcept>

using namespace TrafficControl;

Vehicle::Vehicle(const std::string& vehicleId, const std::string& vehicleType, bool vehicleIsEmergency)
    : id(vehicleId), emergency(vehicleIsEmergency), type(vehicleType) {
        if (id.empty() || type.empty()) {
            throw std::invalid_argument("Vehicle ID and type must not be empty.");
        }
    }

std::string Vehicle::getId() const {
    return id;
}

std::string Vehicle::getType() const {
    return type;
}

bool Vehicle::isEmergencyVehicle() const {
    return emergency;
}