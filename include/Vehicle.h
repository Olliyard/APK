#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>

class Vehicle {
public:
    Vehicle(const std::string& id, const std::string& type);

    std::string getId() const;
    std::string getType() const;

private:
    std::string id;
    std::string type;
};

#endif // VEHICLE_H
