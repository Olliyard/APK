#include "../include/TrafficControlCenter.h"
using namespace TrafficControl;

TrafficControlCenter::TrafficControlCenter() {}

void TrafficControlCenter::addIntersection(std::shared_ptr<Intersection> intersection) {
    std::lock_guard<std::mutex> lock(intersectionsMutex);
    try {
        intersections.push_back(intersection);
    } catch (const std::exception& e) {
        std::cerr << "An error occurred while adding an intersection: " << e.what() << std::endl;
        // Handle the error appropriately
    }
}

void TrafficControlCenter::manageTraffic() {
    std::unique_lock<std::mutex> lock(intersectionsMutex);
    try {
        // Logic to manage traffic lights across intersections. Do so based on time or traffic load.
        for (auto& intersection : intersections) {
            intersection->updateLights();
            std::cout << "Intersection " << intersection->getId() << ": " << intersection->getLightStatus() << std::endl;
            if (intersection->getLightStatus() == "GREEN" && !intersection->isVehicleQueueEmpty()) {
                intersection->processQueue();
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "An error occurred while managing traffic: " << e.what() << std::endl;
        // Handle the error appropriately
    }
    lock.unlock();
    std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Add a short delay to prevent stack overflow
}

void TrafficControlCenter::emergencyVehicleApproach(const std::string& intersectionId) {
    std::lock_guard<std::mutex> lock(intersectionsMutex);
    try {
        for (auto& intersection : intersections) {
            if (intersection->getId() == intersectionId) {
                intersection->processQueue();
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "An error occurred while processing an emergency vehicle approach: " << e.what() << std::endl;
        // Handle the error appropriately
    }
}