#include "../include/Intersection.h"

Intersection::Intersection() : currentLight(TrafficLight::RED), threshold(5) {}

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

void Intersection::addVehicleToQueue(Vehicle* vehicle) {
    std::lock_guard<std::mutex> guard(mtx);
    if (vehicle->isEmergencyVehicle()) {
        vehicleQueue.push_front(vehicle);
    }
    else {
        vehicleQueue.push_back(vehicle);
    }
    sortVehicleQueue();

}

void Intersection::processQueue() {
    std::unique_lock<std::mutex> lock(mtx);
    sortVehicleQueue();
    while (true) {
        // Wait until light is green or there are vehicles in queue and the first vehicle is an emergency vehicle
        cv.wait(lock, [this] { return currentLight == TrafficLight::GREEN || (!vehicleQueue.empty() && vehicleQueue.front()->isEmergencyVehicle()); });

        // Process the vehicle queue
        while (!vehicleQueue.empty() && (currentLight == TrafficLight::GREEN || vehicleQueue.front()->isEmergencyVehicle())) {
            // Remove the vehicle from the queue
            auto vehicle = vehicleQueue.front();
            std::cout << "Vehicle " << vehicle->getId() << " is passing the intersection.\n";
            vehicleQueue.pop_front();
            std::this_thread::sleep_for(std::chrono::milliseconds(500)); // simulate processing time
        }
    }
}


void Intersection::updateLights() {
    auto now = std::chrono::steady_clock::now();
    auto queueSize = vehicleQueue.size();

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
        // Apply settings for LightDuration
        greenDuration = std::chrono::seconds(settings.greenDuration);
        redDuration = std::chrono::seconds(settings.redDuration);
    } else if constexpr (std::is_same<SettingType, PriorityThreshold>::value) {
        // Apply settings for PriorityThreshold
        threshold = settings.vehicleCount;
    } else {
        // Handle other types or throw an exception
        throw std::invalid_argument("Unsupported setting type");
    }
}


template <typename... Settings>
void configureIntersection(Intersection& intersection, Settings... settings) {
    (intersection.applySetting(settings), ...);
}

void Intersection::sortVehicleQueue() {
    std::sort(vehicleQueue.begin(), vehicleQueue.end(), [](const Vehicle* a, const Vehicle* b) {
        // Prioritize emergency vehicles
        if (a->isEmergencyVehicle() && !b->isEmergencyVehicle()) return true;
        if (!a->isEmergencyVehicle() && b->isEmergencyVehicle()) return false;
    });
}
