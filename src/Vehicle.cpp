#include "../include/Vehicle.h"

Vehicle::Vehicle(const std::string& id, const std::string& type, bool isEmergency) : id(id), type(type), emergency(isEmergency){}

std::string Vehicle::getId() const {
    return id;
}

std::string Vehicle::getType() const {
    return type;
}

bool Vehicle::isEmergencyVehicle() const {
    return emergency;
}