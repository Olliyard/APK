#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <iostream>
#include <thread>
#include "Vehicle.h"

class Intersection {
public:
    Intersection();
    void changeLights();
    void addVehicleToQueue(Vehicle* vehicle);
    void processQueue();
    void updateLights();

private:
    enum class TrafficLight { RED, YELLOW, GREEN };
    TrafficLight currentLight;
    std::queue<Vehicle*> vehicleQueue;
    std::mutex mtx;
    std::condition_variable cv;
    std::chrono::steady_clock::time_point lastChange;
    std::chrono::seconds greenDuration;
    std::chrono::seconds redDuration;
    unsigned int threshold; // Number of vehicles that trigger the light change
};

#endif // INTERSECTION_H
