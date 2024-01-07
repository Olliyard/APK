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
    vehicleQueue.push(vehicle);
}

void Intersection::processQueue() {
    // Process the queue based on the current light status
    std::unique_lock<std::mutex> lock(mtx);
    while (true){
        cv.wait(lock, [this] { return currentLight == TrafficLight::GREEN; });

        while (!vehicleQueue.empty() && currentLight == TrafficLight::GREEN) {
            auto vehicle = vehicleQueue.front();
            vehicleQueue.pop();
            // Simulate vehicle passing
            std::cout << "Vehicle " << vehicle->getId() << " is passing the intersection.\n";
            // Add a delay to simulate the time taken for a vehicle to pass
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
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