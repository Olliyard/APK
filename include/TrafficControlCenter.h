#ifndef TRAFFIC_CONTROL_CENTER_H
#define TRAFFIC_CONTROL_CENTER_H

#include <vector>
#include "Intersection.h"
// #include <boost/signals2.hpp>
namespace TrafficControl {
    class TrafficControlCenter {
    public:
        TrafficControlCenter();

        void addIntersection(std::shared_ptr<Intersection> intersection);   // List of intersections managed by the center
        void manageTraffic();
        void emergencyVehicleApproach(const std::string& intersectionId);

    private:
        std::vector<std::shared_ptr<Intersection>> intersections;
        std::mutex intersectionsMutex;
        // boost::signals2::signal<void(std::string)> emergencySignal;
    };
} // namespace TrafficControl
#endif // TRAFFIC_CONTROL_CENTER_H
