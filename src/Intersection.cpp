#include "../include/Intersection.h"

using namespace TrafficControl;

// Define a variant type for vehicles
using VehicleVariant = std::variant<std::unique_ptr<TrafficControl::NormalVehicle>, std::unique_ptr<TrafficControl::EmergencyVehicle>>;

// Change the vehicleQueue to use VehicleVariant
std::deque<VehicleVariant> vehicleQueue;

// Add a mutex for the vehicleQueue
Intersection::Intersection(const std::string &intersectionId) : id(intersectionId), currentLight(TrafficLight::RED), threshold(4)
{
    queueProcessingThread = std::thread(&Intersection::processQueue, this);
}

// Return the id of the intersection
std::string Intersection::getId() const
{
    return id;
}

// Change the lights
void Intersection::changeLights()
{
    for (int i = 0; i < 3; ++i)
    {
        try
        {
            if (currentLight == TrafficLight::GREEN)
            {
                currentLight = TrafficLight::RED;
                std::cout << "Intersection " << getId() << ": Light: RED" << std::endl; // Print out the new light condition
            }
            else
            {
                currentLight = TrafficLight::GREEN;
                std::cout << "Intersection " << getId() << ": Light: GREEN" << std::endl; // Print out the new light condition
            }
            return;
        }
        catch (const std::exception &e)
        {
            std::cerr << "An error occurred while changing the lights: " << e.what() << std::endl;
            if (i == 2)
            {
                throw;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

// Add a vehicle to the queue
void Intersection::addVehicleToQueue(VehicleVariant vehicle)
{
    for (int i = 0; i < 3; ++i)
    { // Try the operation 3 times
        try
        {
            std::lock_guard<std::mutex> guard(mtx);
            if (std::holds_alternative<std::unique_ptr<EmergencyVehicle>>(vehicle))
            {
                vehicleQueue.push_front(std::move(vehicle));
            }
            else
            {
                vehicleQueue.push_back(std::move(vehicle));
            }
            sortVehicleQueue();
            cv.notify_all(); // Notify all waiting threads

            // Debug output
            std::cout << "Vehicle added to queue at intersection " << getId() << ". Queue size: " << vehicleQueue.size() << std::endl;

            return;          // If the operation succeeded, return immediately
        }
        catch (const std::exception &e)
        {
            std::cerr << "An error occurred while adding a vehicle to the queue: " << e.what() << std::endl;
            if (i == 2)
            { // If this was the last attempt, rethrow the exception
                throw;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Wait before retrying
        }
    }
    cv.notify_all(); // Notify all waiting threads
}

// Process the queue
void Intersection::processQueue()
{
    while (!stopProcessing)
    {
        VehicleVariant vehicle;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this]
                    { return !vehicleQueue.empty() || stopProcessing; });
            if (stopProcessing)
            {
                break;
            }

            // Check if the vehicle at the front of the queue is allowed to proceed
            bool isEmergencyVehicle = std::visit([](auto&& arg) { return arg->isEmergencyVehicle(); }, vehicleQueue.front());
            if (currentLight == TrafficLight::GREEN || isEmergencyVehicle) {
                vehicle = std::move(vehicleQueue.front());
                vehicleQueue.pop_front();
            } else {
                // If the light is red and the vehicle is not an emergency vehicle, do not remove it from the queue
                continue;
            }
        }

        std::visit([&](auto &v)
                   {
            std::lock_guard<std::mutex> lock(mtx);
            v->proceed(this->getId());
                   }, vehicle);
    }
}

// update the lights based on the queue size
void Intersection::updateLights()
{
    for (int i = 0; i < 3; ++i)
    { // Try the operation 3 times
        try
        {
            auto now = std::chrono::steady_clock::now();
            auto queueSize = vehicleQueue.size();

            // Calculate green and red durations based on queue size
            greenDuration = std::chrono::seconds(static_cast<std::chrono::seconds::rep>(settings["greenDuration"]) + static_cast<std::chrono::seconds::rep>(queueSize - threshold));
            redDuration = std::chrono::seconds(static_cast<std::chrono::seconds::rep>(settings["redDuration"]) - static_cast<std::chrono::seconds::rep>(queueSize - threshold));

            std::lock_guard<std::mutex> lock(mtx);
            if (currentLight == TrafficLight::GREEN)
            {
                // If the light has been green for defined time
                if (now - lastChange >= greenDuration)
                {
                    // Change the light to RED
                    currentLight = TrafficLight::RED;
                    std::cout << "Intersection " << getId() << ": Light: RED" << std::endl; // Print out the new light condition
                    lastChange = now;
                    cv.notify_all(); // Notify all waiting threads
                }
            }
            else if (currentLight == TrafficLight::RED)
            {
                if (now - lastChange >= redDuration)
                {
                    // Change the light to GREEN
                    currentLight = TrafficLight::GREEN;
                    std::cout << "Intersection " << getId() << ": Light: GREEN" << std::endl; // Print out the new light condition
                    lastChange = now;
                    cv.notify_all(); // Notify all waiting threads
                }
            }
            return; // If the operation succeeded, return immediately
        }
        catch (const std::exception &e)
        {
            std::cerr << "An error occurred while updating the lights: " << e.what() << std::endl;
            if (i == 2)
            { // If this was the last attempt, rethrow the exception
                throw;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Wait before retrying
        }
    }
}

// Sort the vehicle queue based on emergency vehicles
void Intersection::sortVehicleQueue()
{
    try
    {
        std::sort(vehicleQueue.begin(), vehicleQueue.end(), [](const VehicleVariant &a, const VehicleVariant &b)
                  {
            bool aIsEmergency = std::visit([](auto&& arg) { return arg->isEmergencyVehicle(); }, a);
            bool bIsEmergency = std::visit([](auto&& arg) { return arg->isEmergencyVehicle(); }, b);
            return aIsEmergency && !bIsEmergency; });
    }
    catch (const std::exception &e)
    {
        std::cerr << "An error occurred while sorting the vehicle queue: " << e.what() << std::endl;
        // Handle the error appropriately
    }
}

// Return the light status
std::string Intersection::getLightStatus()
{
    std::lock_guard<std::mutex> lock(mtx);
    std::string lightStatus = currentLight == TrafficLight::GREEN ? "GREEN" : "RED";
    // auto timeUntilNextLight = /* logic to calculate time until next light */;
    return "Light: " + lightStatus;
    // return "Light: " + lightStatus + ", Time until next light: " + std::to_string(timeUntilNextLight);
}