#ifndef TRAFFIC_CONTROL_CENTER_H
#define TRAFFIC_CONTROL_CENTER_H

#include <vector>
#include "Intersection.h"
// #include <boost/signals2.hpp>

class TrafficControlCenter {
public:
    TrafficControlCenter();

    void addIntersection(Intersection* intersection);   // List of intersections managed by the center
    void manageTraffic();
    void emergencyVehicleApproach(const std::string& intersectionId);

private:
    std::vector<Intersection*> intersections;
    // boost::signals2::signal<void(std::string)> emergencySignal;
};

#endif // TRAFFIC_CONTROL_CENTER_H
