#include "../include/Intersection.h"

using namespace TrafficControl;

Intersection::Intersection(const std::string& intersectionId) : id(intersectionId), currentLight(TrafficLight::RED), threshold(3) {}

std::string Intersection::getId() const {
    return id;
}

void Intersection::changeLights() {
    try {
        // Logic to change the traffic lights
        auto now = std::chrono::steady_clock::now();
        if (currentLight == TrafficLight::GREEN && now - lastChange > greenDuration) {
            currentLight = TrafficLight::RED;
            lastChange = now;
            cv.notify_all(); // Notify all waiting threads
        } else if (currentLight == TrafficLight::RED && now - lastChange > redDuration) {
            currentLight = TrafficLight::GREEN;
            lastChange = now;
            cv.notify_all(); // Notify all waiting threads
        }
    } catch (const std::exception& e) {
        std::cerr << "An error occurred while changing the lights: " << e.what() << std::endl;
        // Handle the error appropriately
    }
}

void Intersection::addVehicleToQueue(std::unique_ptr<Vehicle> vehicle) {
    try {
        std::lock_guard<std::mutex> guard(mtx);
        if (vehicle->isEmergencyVehicle()) {
            vehicleQueue.push_front(std::move(vehicle));
        }
        else {
            vehicleQueue.push_back(std::move(vehicle));
        }
        sortVehicleQueue();
        cv.notify_all(); // Notify all waiting threads
    } catch (const std::exception& e) {
        std::cerr << "An error occurred while adding a vehicle to the queue: " << e.what() << std::endl;
        // Handle the error appropriately
    }
}

void Intersection::processQueue() {
    std::unique_lock<std::mutex> lock(mtx);
    while (true) {
        cv.wait(lock, [this] { 
            return !vehicleQueue.empty();
        });

        while (!vehicleQueue.empty()) {
            // Process the vehicle at the front of the queue
            auto& vehicle = vehicleQueue.front();

            // If the light is red and the vehicle is not an emergency vehicle, wait for the light to turn green
            if (!vehicle->isEmergencyVehicle() && currentLight == TrafficLight::RED) {
                cv.wait(lock, [this] { 
                    return currentLight == TrafficLight::GREEN;
                });
            }

            if (vehicle->isEmergencyVehicle() || currentLight == TrafficLight::GREEN) {
                std::cout << "Vehicle " << vehicle->getId() << " is passing the intersection.\n";
                vehicleQueue.pop_front();
                std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Simulate processing time
            }

            if (vehicleQueue.empty() || currentLight != TrafficLight::GREEN) {
                break; // Stop processing if the light is not green and no emergency vehicle is waiting
            }
        }
    }
}

void Intersection::updateLights() {
    try {
        auto now = std::chrono::steady_clock::now();
        auto queueSize = vehicleQueue.size();
        greenDuration = std::chrono::seconds(settings["greenDuration"]);
        redDuration = std::chrono::seconds(settings["redDuration"]);

        if (currentLight == TrafficLight::GREEN) {
            // If there are vehicles in the queue and the light has been green for defined time
            if (queueSize >= threshold && now - lastChange < greenDuration) {
                return;
            }
            // Change the light to RED
            changeLights();
            lastChange = now;
            cv.notify_all(); // Notify all waiting threads
        }
        else if (currentLight == TrafficLight::RED) {
            if (now - lastChange >= redDuration || queueSize >= threshold) {
                changeLights(); // to GREEN
                lastChange = now;
                cv.notify_all(); // Notify all waiting threads
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "An error occurred while updating the lights: " << e.what() << std::endl;
        // Handle the error appropriately
    }
}

void Intersection::sortVehicleQueue() {
    try {
        std::sort(vehicleQueue.begin(), vehicleQueue.end(), [](const std::unique_ptr<Vehicle>& a, const std::unique_ptr<Vehicle>& b) {
            if (a->isEmergencyVehicle() && !b->isEmergencyVehicle()) return true;
            if (!a->isEmergencyVehicle() && b->isEmergencyVehicle()) return false;
            return false;
        });
    } catch (const std::exception& e) {
        std::cerr << "An error occurred while sorting the vehicle queue: " << e.what() << std::endl;
        // Handle the error appropriately
    }
}

std::string Intersection::getLightStatus() {
    std::lock_guard<std::mutex> lock(mtx);
    std::string lightStatus = currentLight == TrafficLight::GREEN ? "GREEN" : "RED";
    // auto timeUntilNextLight = /* logic to calculate time until next light */;
    return "Light: " + lightStatus;
    // return "Light: " + lightStatus + ", Time until next light: " + std::to_string(timeUntilNextLight);
}