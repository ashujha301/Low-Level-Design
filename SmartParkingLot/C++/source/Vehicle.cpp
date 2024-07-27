#include "headers/Vehicle.hpp"
#include "headers/ParkingSpot.hpp"
using namespace std;


void Vehicle::removeVehicle()  {
    spot->removeVehicle();
    spot=NULL;
};