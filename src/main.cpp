#include "../include/TrafficControlCenter.h"
#include "../include/Intersection.h"
#include "../include/Vehicle.h"
#include <iostream>
#include <exception>

using namespace TrafficControl;
// std::mutex consoleMtx; // Mutex to protect console output

int main()
{
    try
    {
        std::cout << "Initializing traffic control system..." << std::endl;
        TrafficControlCenter controlCenter;
        std::shared_ptr<Intersection> intersection1 = std::make_shared<Intersection>("1");
        std::shared_ptr<Intersection> intersection2 = std::make_shared<Intersection>("2");

        controlCenter.addIntersection(intersection1);
        controlCenter.addIntersection(intersection2);
        std::cout << "Traffic control center and intersections initialized." << std::endl;

        std::thread trafficThread([&controlCenter]()
                                  {
            try {
                std::cout << "Traffic management thread started." << std::endl;
                while (true) {
                    controlCenter.manageTraffic();
                    // std::cout << "Traffic management thread is sleeping." << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }
            } catch (const std::exception& e) {
                std::cerr << "Error in traffic management thread: " << e.what() << std::endl;
            } });

        // std::thread intersection1Thread([&intersection1]()
        //                                 {
        //     while (true)
        //     {
        //         intersection1->processQueue();
        //         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        //     }
        // });

        // std::thread intersection2Thread([&intersection2]()
        //                                 {
        //     while (true)
        //     {
        //         intersection2->processQueue();
        //         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        //     }
        // });


        int vehicleCounter = 0;
        intersection1->applySetting(LightDuration(5, 5));
        intersection2->applySetting(LightDuration(10, 10));

        while (true)
        {
            vehicleCounter++;
            std::string vehicleId = std::to_string(vehicleCounter);
            std::unique_ptr<TrafficControl::NormalVehicle> newVehicle1 = std::make_unique<TrafficControl::NormalVehicle>(vehicleId, "Normal");
            std::string vehicle1ID = newVehicle1->getId();
            intersection1->addVehicleToQueue(std::move(newVehicle1));

            vehicleCounter++;
            vehicleId = std::to_string(vehicleCounter);
            std::unique_ptr<TrafficControl::NormalVehicle> newVehicle2 = std::make_unique<TrafficControl::NormalVehicle>(vehicleId, "Normal");
            std::string vehicle2ID = newVehicle2->getId();
            intersection2->addVehicleToQueue(std::move(newVehicle2));
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        trafficThread.join();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error in main thread: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}