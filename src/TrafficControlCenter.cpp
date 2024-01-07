#include "../include/TrafficControlCenter.h"

TrafficControlCenter::TrafficControlCenter() {}

void TrafficControlCenter::addIntersection(Intersection* intersection) {
    intersections.push_back(intersection);
}

void TrafficControlCenter::manageTraffic() {
    // Logic to manage traffic lights across intersections. Do so based on time or traffic load.
    for (auto& intersection : intersections) {
        intersection->updateLights();
    }
}

void TrafficControlCenter::emergencyVehicleApproach(const std::string& intersectionId) {
    emergencySignal(intersectionId);
}
