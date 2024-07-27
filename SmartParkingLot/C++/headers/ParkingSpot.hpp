#pragma once
#include "Vehicle.hpp"
#include "Level.hpp"


class ParkingSpot {

    Vehicle *v;
    VehicleSize spotSize;
    int spotNum;
    Level *1;

public:
    ParkingSpot(Level *1,int n,VehicleSize sz) : l(1),spotNum(n),spotSize(sz) {
        v=NULL;
    }

    bool isAvailable(){
        return v==NULL;
    }

    bool canVehicleFit(Vehicle *v){
        return (v->getSize() == spotSize);
    }

    bool ParkVehicle(Vehicle *v);
    void removeVehicle();
};