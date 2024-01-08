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

namespace TrafficControl {

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
        std::string id;
        Intersection(const std::string& id);
        bool isVehicleQueueEmpty() {
            std::lock_guard<std::mutex> lock(mtx);
            return vehicleQueue.empty();
        }
        std::string getId() const;
        std::map<std::string, int> settings;
        void changeLights();
        void addVehicleToQueue(std::unique_ptr<Vehicle> vehicle);
        void processQueue();
        void updateLights();
        template <typename SettingType>
        void applySetting(const SettingType& newSettings) {
            try {
                if constexpr (std::is_same<SettingType, LightDuration>::value) {
                    this->settings["greenDuration"] = newSettings.greenDuration;
                    this->settings["redDuration"] = newSettings.redDuration;
                } else if constexpr (std::is_same<SettingType, PriorityThreshold>::value) {
                    this->settings["vehicleCount"] = newSettings.vehicleCount;
                } else {
                    throw std::invalid_argument("Unsupported setting type");
                }
            } catch (const std::exception& e) {
                std::cerr << "An error occurred while applying settings: " << e.what() << std::endl;
                // Handle the error appropriately
            }
        }
        void sortVehicleQueue();
        std::string getLightStatus();

    private:
        enum class TrafficLight { RED, YELLOW, GREEN };
        TrafficLight currentLight;
        std::deque<std::unique_ptr<Vehicle>> vehicleQueue;
        std::mutex mtx;
        std::condition_variable cv;
        std::chrono::steady_clock::time_point lastChange;
        std::chrono::seconds greenDuration;
        std::chrono::seconds redDuration;
        unsigned int threshold; // Number of vehicles that trigger the light change
    };
} // namespace TrafficControl
#endif // INTERSECTION_H