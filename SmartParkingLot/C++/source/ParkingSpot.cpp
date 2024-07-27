#include "headers/ParkingSpot.hpp"
#include "headers/Vehicle.hpp"
#include <iostream>
using namespace std;

void ParkingSpot:: removeVehicle(){
    v=NULL;
    l->spotFreed(spotSize);
}

bool ParkingSpot:: ParkVehicle(Vehicle *v){
    if(!canVehicleFit(v) || !isAvailable()){
        return false;
    }

    v=v;
    v->parkInSpot(this);
    return true;
}