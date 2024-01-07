#include "../include/TrafficControlCenter.h"
#include "../include/Intersection.h"
#include "../include/Vehicle.h"

int main(){
    TrafficControlCenter controlCenter;
    Intersection intersection1("1");

    controlCenter.addIntersection(&intersection1);

    // Start a thread to manage traffic
    std::thread trafficThread([&controlCenter](){
        while (true){
            controlCenter.manageTraffic();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });

    // Simulate some traffic
    int vehicleCounter = 0;
    while(true){

        std::unique_ptr<Vehicle> newVehicle = std::make_unique<Vehicle>(std::to_string(vehicleCounter++), "Car", false);
        intersection1.addVehicleToQueue(std::move(newVehicle));

        // Simulate a vehicle arriving at the intersection every two seconds
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    trafficThread.join();
    return 0;
}