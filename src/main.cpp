#include "../include/TrafficControlCenter.h"
#include "../include/Intersection.h"
#include "../include/Vehicle.h"
#include <iostream>
#include <exception>

using namespace TrafficControl;

int main() {
    try {
        std::cout << "Initializing traffic control system..." << std::endl;
        TrafficControlCenter controlCenter;
        std::shared_ptr<Intersection> intersection1 = std::make_shared<Intersection>("1");
        std::shared_ptr<Intersection> intersection2 = std::make_shared<Intersection>("2");

        controlCenter.addIntersection(intersection1);
        controlCenter.addIntersection(intersection2);
        std::cout << "Traffic control center and intersections initialized." << std::endl;

        std::thread trafficThread([&controlCenter]() {
            try {
                std::cout << "Traffic management thread started." << std::endl;
                while (true) {
                    controlCenter.manageTraffic();
                    // std::cout << "Traffic management thread is sleeping." << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }
            } catch (const std::exception& e) {
                std::cerr << "Error in traffic management thread: " << e.what() << std::endl;
            }
        });

        int vehicleCounter = 0;

        intersection1->settings["greenDuration"] = 3;
        intersection1->settings["redDuration"] = 3;
        intersection2->settings["greenDuration"] = 3;
        intersection2->settings["redDuration"] = 3;
        intersection1->applySetting(LightDuration(3, 3));
        intersection2->applySetting(LightDuration(3, 3));

        while (true) {
            std::unique_ptr<Vehicle> newVehicle1 = std::make_unique<Vehicle>(std::to_string(vehicleCounter++), "Car", false);
            std::unique_ptr<Vehicle> newVehicle2 = std::make_unique<Vehicle>(std::to_string(vehicleCounter++), "Ambulance", true);
            intersection1->addVehicleToQueue(std::move(newVehicle1));
            intersection2->addVehicleToQueue(std::move(newVehicle2));
            std::cout << "Vehicle added to queue at intersection " << intersection1->getId() << "." << std::endl;
            std::cout << "Vehicle added to queue at intersection " << intersection2->getId() << "." << std::endl;

            std::this_thread::sleep_for(std::chrono::seconds(2));
        }

        trafficThread.join();
    } catch (const std::exception& e) {
        std::cerr << "Error in main thread: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}