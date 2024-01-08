#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>
namespace TrafficControl {
    class Vehicle {
    public:
        Vehicle(const std::string& id, const std::string& type, bool isEmergency = false);

        std::string getId() const;
        std::string getType() const;
        bool isEmergencyVehicle() const;

    private:
        std::string id;
        bool emergency;
        std::string type;
    };
} // namespace TrafficControl
#endif // VEHICLE_H
