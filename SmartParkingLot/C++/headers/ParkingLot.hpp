#pragma once
#include "Level.hpp"

class ParkingLot {
    vector<Level*> floors;
    static const int Max_FLOORS = 2;
public:
    ParkingLot();
    bool ParkVehicle(Vehicle *v);
    void displayFreeSlots();
};