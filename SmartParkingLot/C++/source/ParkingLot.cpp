#include "headers/ParkingLot.hpp"
using namespace std;

ParkingLot::ParkingLot(){

    for(int i=0;i<Max_FLOORS;i++){
        floors.push_back(new Level(i,12));
    }
}

bool ParkingLot:: ParkVehicle(Vehicle *v){

    for(int i=0;i<Max_FLOORS;i++){
        if(floors[i]->ParkVehicle()){
            return true;
        }
    }

    return false;
}

void ParkingLot::displayFreeSlots(){

    for(int i=0;i<floors.size();i++){
        floors[i]->displayAvailableSlots();
        cout<<endl;
    }
}