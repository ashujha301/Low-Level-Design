#pragma once
#include "ParkingSpot.hpp"
class Level {
    int Floor;
    vector<ParkingSpot*> spots;
    int availableSpots[3];
public:

    Level(int f,int numSpots);

    void spotFreed(VehicleSize size){
        availableSpots[size]++;
    }

    bool ParkVehicle(Vehicle *v);
    void displayAvailableSlots();
};