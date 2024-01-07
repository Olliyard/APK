#include "../include/Intersection.h"

Intersection::Intersection(const std::string& id) : id(id), currentLight(TrafficLight::RED), threshold(5) {}

std::string Intersection::getId() const {
    return id;
}

void Intersection::changeLights() {
    // Logic to change the traffic lights
    auto now = std::chrono::steady_clock::now();
    if (currentLight == TrafficLight::GREEN && now - lastChange > greenDuration) {
        changeLights(); // to RED
        lastChange = now;
    } else if (currentLight == TrafficLight::RED && now - lastChange > redDuration) {
        changeLights(); // to GREEN
        lastChange = now;
    }
}

void Intersection::addVehicleToQueue(std::unique_ptr<Vehicle> vehicle) {
    std::lock_guard<std::mutex> guard(mtx);
    if (vehicle->isEmergencyVehicle()) {
        vehicleQueue.push_front(std::move(vehicle));
    }
    else {
        vehicleQueue.push_back(std::move(vehicle));
    }
    sortVehicleQueue();

}

void Intersection::processQueue() {
    std::unique_lock<std::mutex> lock(mtx);
    while (true) {
        cv.wait(lock, [this] { 
            return !vehicleQueue.empty() && 
                   (vehicleQueue.front()->isEmergencyVehicle() || currentLight == TrafficLight::GREEN);
        });

        while (!vehicleQueue.empty() && 
               (vehicleQueue.front()->isEmergencyVehicle() || currentLight == TrafficLight::GREEN)) {
            // Process the vehicle at the front of the queue
            auto& vehicle = vehicleQueue.front();
            std::cout << "Vehicle " << vehicle->getId() << " is passing the intersection.\n";
            vehicleQueue.pop_front();
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Simulate processing time

            if (vehicleQueue.empty() || currentLight != TrafficLight::GREEN) {
                break; // Stop processing if the light is not green and no emergency vehicle is waiting
            }
        }
    }
}



void Intersection::updateLights() {
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
    } else if (currentLight == TrafficLight::RED) {
        // If there are are vehicles in the queue and the light has been red for defined time
        if (now - lastChange >= redDuration) {
            changeLights(); // to GREEN
            lastChange = now;
        }
    }
}

template <typename SettingType>
void Intersection::applySetting(const SettingType& settings) {
    if constexpr (std::is_same<SettingType, LightDuration>::value) {
        this->settings["greenDuration"] = settings.greenDuration;
        this->settings["redDuration"] = settings.redDuration;
    } else if constexpr (std::is_same<SettingType, PriorityThreshold>::value) {
        this->settings["vehicleCount"] = settings.vehicleCount;
    } else {
        throw std::invalid_argument("Unsupported setting type");
    }
}


template <typename... Settings>
void configureIntersection(Intersection& intersection, Settings... settings) {
    (intersection.applySetting(settings), ...);
}

void Intersection::sortVehicleQueue() {
    std::sort(vehicleQueue.begin(), vehicleQueue.end(), [](const Vehicle* a, const Vehicle* b) {
        if (a->isEmergencyVehicle() && !b->isEmergencyVehicle()) return true;
        if (!a->isEmergencyVehicle() && b->isEmergencyVehicle()) return false;
        return false;
    });
}
