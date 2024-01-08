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
#include <variant>
#include <future>
#include <functional>
#include "Vehicle.h"

namespace TrafficControl
{

    class NormalVehicle;
    class EmergencyVehicle;

    // Define a variant type for vehicles
    using VehicleVariant = std::variant<std::unique_ptr<NormalVehicle>, std::unique_ptr<EmergencyVehicle>>;

    struct LightDuration
    {
        int greenDuration;
        int redDuration;
        LightDuration(int green, int red) : greenDuration(green), redDuration(red) {}
    };

    struct PriorityThreshold
    {
        int vehicleCount;
        PriorityThreshold(int count) : vehicleCount(count) {}
    };

    class Intersection
    {
        std::thread queueProcessingThread;
        bool stopProcessing = false;

    public:
        enum class TrafficLight
        {
            RED,
            GREEN
        };
        std::string id;
        std::mutex mtx;
        Intersection(const std::string &id);
        ~Intersection()
        {
            stopProcessing = true;
            cv.notify_all();
            queueProcessingThread.join();
        }

        size_t getQueueSize() const
        {
            return vehicleQueue.size();
        }

        bool isVehicleQueueEmpty()
        {
            std::lock_guard<std::mutex> lock(mtx);
            return vehicleQueue.empty();
        }
        std::string getId() const;
        std::map<std::string, int> settings;
        void changeLights();
        void addVehicleToQueue(VehicleVariant vehicle);
        void processQueue();
        void updateLights();
        template <typename SettingType>
        void applySetting(const SettingType &newSettings)
        {
            try
            {
                if constexpr (std::is_same<SettingType, LightDuration>::value)
                {
                    this->settings["greenDuration"] = newSettings.greenDuration;
                    this->settings["redDuration"] = newSettings.redDuration;
                }
                else if constexpr (std::is_same<SettingType, PriorityThreshold>::value)
                {
                    this->settings["vehicleCount"] = newSettings.vehicleCount;
                }
                else
                {
                    throw std::invalid_argument("Unsupported setting type");
                }
            }
            catch (const std::exception &e)
            {
                std::cerr << "An error occurred while applying settings: " << e.what() << std::endl;
                // Handle the error appropriately
            }
        }
        void sortVehicleQueue();
        std::string getLightStatus();
        std::mutex &getMutex() { return mtx; }
        TrafficLight getCurrentLight() { return currentLight; }
        void setCurrentLight(TrafficLight light) { currentLight = light; }
        std::chrono::seconds getRedDuration() { return redDuration; }
        void setRedDuration(std::chrono::seconds duration) { redDuration = duration; }
        std::chrono::seconds getGreenDuration() { return greenDuration; }
        void setGreenDuration(std::chrono::seconds duration) { greenDuration = duration; }

    private:
        TrafficLight currentLight;
        std::deque<VehicleVariant> vehicleQueue;
        std::condition_variable cv;
        std::chrono::steady_clock::time_point lastChange;
        std::chrono::seconds greenDuration;
        std::chrono::seconds redDuration;
        unsigned int threshold; // Number of vehicles that trigger the light change
    };
} // namespace TrafficControl
#endif // INTERSECTION_H