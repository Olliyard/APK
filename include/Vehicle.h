#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>
#include <iostream>

namespace TrafficControl
{
    class Vehicle
    {
    public:
        Vehicle(const std::string &id, const std::string &type, bool isEmergency = false);

        std::string getId() const;
        std::string getType() const;
        bool isEmergencyVehicle() const;
        void proceed(const std::string& intersectionId);

    private:
        std::string id;
        bool emergency;
        std::string type;
    };

    class NormalVehicle : public Vehicle
    {
    public:
        NormalVehicle(const std::string &vehicleId, const std::string &vehicleType) : Vehicle(vehicleId, vehicleType, false) {}
    };

    class EmergencyVehicle : public Vehicle
    {
    public:
        EmergencyVehicle(const std::string &vehicleId, const std::string &vehicleType) : Vehicle(vehicleId, vehicleType, true) {}
    };
} // namespace TrafficControl
#endif // VEHICLE_H
