#pragma once
#include <string>
using namespace std;

#include "headers/common.hpp";
class ParkingSpot;
class Level;

class Vehicle {
    ParkingSpot *spot;
    string numPlate;
    VehicleSize vs;
public:

    Vehicle(string num,VehicleSize size) : numPlate(num),vs(size) {}
    VehicleSize getSize() const {
        return vs;
    }

    void parkInSpot(ParkingSpot *ps){
        spot=ps;
    }

    void removeVehicle();
};