#include "../include/Vehicle.h"
#include <stdexcept>
#include <thread>

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

void Vehicle::proceed(const std::string& intersectionId) {
    if (isEmergencyVehicle()) {
        std::cout << "Emergency vehicle " << id << " is proceeding through intersection " << intersectionId << "." << std::endl;
    } else {
        std::cout << "Vehicle " << id << " is proceeding through intersection " << intersectionId << "." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}