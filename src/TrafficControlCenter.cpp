#include "../include/TrafficControlCenter.h"
using namespace TrafficControl;

TrafficControlCenter::TrafficControlCenter() {}

// Add an intersection to the traffic control center
void TrafficControlCenter::addIntersection(std::shared_ptr<Intersection> intersection)
{
    std::lock_guard<std::mutex> lock(intersectionsMutex);   // Lock the intersections vector
    try
    {
        intersections.push_back(intersection);              // Add the intersection to the vector
    }
    catch (const std::exception &e)
    {
        std::cerr << "An error occurred while adding an intersection: " << e.what() << std::endl;
        // Handle the error appropriately
    }
}

// Manage traffic for each intersection in the traffic control center
void TrafficControlCenter::manageTraffic()
{
    std::lock_guard<std::mutex> lock(intersectionsMutex);
    try
    {
        for (auto &intersection : intersections)
        {
            intersection->updateLights();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "An error occurred while managing traffic: " << e.what() << std::endl;
        // Handle the error appropriately
    }
}


// Process an emergency vehicle approach
void TrafficControlCenter::emergencyVehicleApproach(const std::string &intersectionId)
{
    std::lock_guard<std::mutex> lock(intersectionsMutex);
    try
    {
        for (auto &intersection : intersections)
        {
            if (intersection->getId() == intersectionId)
            {
                intersection->processQueue();
                break;
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "An error occurred while processing an emergency vehicle approach: " << e.what() << std::endl;
        // Handle the error appropriately
    }
}