#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <map>
#include <algorithm>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <iostream>
#include <thread>
#include "Vehicle.h"

struct LightDuration {
    int greenDuration;
    int redDuration;
    LightDuration(int green, int red) : greenDuration(green), redDuration(red) {}
};

struct PriorityThreshold {
    int vehicleCount;
    PriorityThreshold(int count) : vehicleCount(count) {}
};

class Intersection {
public:
    Intersection();
    std::map<std::string, int> settings;
    void changeLights();
    void addVehicleToQueue(Vehicle* vehicle);
    void processQueue();
    void updateLights();
    template <typename SettingType>
    void applySetting(const SettingType& settings);
    void sortVehicleQueue();

private:
    enum class TrafficLight { RED, YELLOW, GREEN };
    TrafficLight currentLight;
    std::deque<Vehicle*> vehicleQueue;
    std::mutex mtx;
    std::condition_variable cv;
    std::chrono::steady_clock::time_point lastChange;
    std::chrono::seconds greenDuration;
    std::chrono::seconds redDuration;
    unsigned int threshold; // Number of vehicles that trigger the light change
};

#endif // INTERSECTION_H