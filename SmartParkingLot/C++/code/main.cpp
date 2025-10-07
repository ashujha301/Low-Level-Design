#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <chrono>
#include <cmath>
#include <mutex>
#include <thread>
using namespace std;

// ==================== ENUMS ====================
enum class VehicleSize { Motorcycle, Car, Bus };
enum class SpotSize { Small, Medium, Large };
enum class TicketStatus { OPEN, CLOSED };

// Helper
string to_string(VehicleSize v) {
    if (v == VehicleSize::Motorcycle) return "Motorcycle";
    if (v == VehicleSize::Car) return "Car";
    return "Bus";
}
string to_string(SpotSize s) {
    if (s == SpotSize::Small) return "Small";
    if (s == SpotSize::Medium) return "Medium";
    return "Large";
}

// ==================== VEHICLE ====================
class Vehicle {
    string plate;
    VehicleSize size;
public:
    Vehicle(string p, VehicleSize s) : plate(p), size(s) {}
    string getPlate() const { return plate; }
    VehicleSize getSize() const { return size; }
};

// Factory Pattern
class VehicleFactory {
public:
    static Vehicle createVehicle(string type, string plate) {
        if (type == "car") return Vehicle(plate, VehicleSize::Car);
        if (type == "bus") return Vehicle(plate, VehicleSize::Bus);
        return Vehicle(plate, VehicleSize::Motorcycle);
    }
};

// ==================== PARKING SPOT ====================
class ParkingSpot {
    string id;
    SpotSize size;
    bool occupied = false;
    string plate;
public:
    ParkingSpot(string id, SpotSize s): id(id), size(s) {}
    string getId() const { return id; }
    SpotSize getSize() const { return size; }
    bool isAvailable() const { return !occupied; }

    void occupy(string p) { occupied = true; plate = p; }
    void free() { occupied = false; plate.clear(); }
};

// ==================== PARKING LOT ====================
class ParkingLot {
    unordered_map<string, ParkingSpot> spots;
    unordered_map<SpotSize, unordered_set<string>> available;
public:
    void addSpot(ParkingSpot s) {
        spots[s.getId()] = s;
        available[s.getSize()].insert(s.getId());
    }

    optional<string> allocateSpot(Vehicle v) {
        SpotSize wanted = (v.getSize() == VehicleSize::Motorcycle) ? SpotSize::Small :
                          (v.getSize() == VehicleSize::Car) ? SpotSize::Medium : SpotSize::Large;

        // Best fit search
        vector<SpotSize> order = {SpotSize::Small, SpotSize::Medium, SpotSize::Large};
        for (auto sz : order) {
            if (wanted == SpotSize::Small || (wanted == SpotSize::Medium && sz != SpotSize::Small) || (wanted == SpotSize::Large && sz == SpotSize::Large)) {
                if (!available[sz].empty()) {
                    string sid = *available[sz].begin();
                    available[sz].erase(sid);
                    spots[sid].occupy(v.getPlate());
                    return sid;
                }
            }
        }
        return nullopt;
    }

    void freeSpot(string id) {
        if (spots.find(id) != spots.end()) {
            SpotSize s = spots[id].getSize();
            spots[id].free();
            available[s].insert(id);
        }
    }

    void printAvailability() {
        cout << "Availability: ";
        for (auto &p : available)
            cout << to_string(p.first) << "=" << p.second.size() << " ";
        cout << endl;
    }
};

// ==================== PARKING TICKET ====================
class ParkingTicket {
public:
    string id;
    string spotId;
    string plate;
    VehicleSize vsize;
    chrono::system_clock::time_point entryTime;
    chrono::system_clock::time_point exitTime;
    int fee = 0;
    TicketStatus status = TicketStatus::OPEN;

    ParkingTicket(string tid, string sid, string p, VehicleSize s)
        : id(tid), spotId(sid), plate(p), vsize(s) {
        entryTime = chrono::system_clock::now();
    }
};

// ==================== FEE CALCULATOR (Strategy Pattern) ====================
class IFeeCalculator {
public:
    virtual int calculate(const ParkingTicket &t) = 0;
};

class TimeBasedFeeCalculator : public IFeeCalculator {
public:
    int calculate(const ParkingTicket &t) override {
        using namespace chrono;
        int mins = duration_cast<minutes>(t.exitTime - t.entryTime).count();
        int hours = ceil(mins / 60.0);
        if (t.vsize == VehicleSize::Motorcycle) return hours * 10;
        if (t.vsize == VehicleSize::Car) return hours * 20;
        return hours * 50;
    }
};

// ==================== OBSERVER ====================
class IObserver {
public:
    virtual void onUpdate() = 0;
};

class ConsoleObserver : public IObserver {
public:
    void onUpdate() override {
        cout << "[Observer] Availability changed!" << endl;
    }
};

// ==================== DATA STORE ====================
class DataStore {
    unordered_map<string, ParkingTicket> tickets;
    int counter = 1;
    mutex m;
public:
    string createTicket(string spotId, Vehicle v) {
        lock_guard<mutex> lock(m);
        string id = "T" + to_string(counter++);
        tickets[id] = ParkingTicket(id, spotId, v.getPlate(), v.getSize());
        return id;
    }

    optional<ParkingTicket*> getTicket(string id) {
        lock_guard<mutex> lock(m);
        if (tickets.find(id) == tickets.end()) return nullopt;
        return &tickets[id];
    }
};

// ==================== MANAGER (Facade Pattern) ====================
class ParkingManager {
    ParkingLot lot;
    DataStore store;
    TimeBasedFeeCalculator calc;
    vector<IObserver*> observers;

    string newTicketId(string spotId, Vehicle v) { return store.createTicket(spotId, v); }
    void notify() { for (auto o : observers) o->onUpdate(); }

public:
    void addSpot(string id, SpotSize size) { lot.addSpot(ParkingSpot(id, size)); }
    void registerObserver(IObserver* o) { observers.push_back(o); }

    optional<string> checkIn(Vehicle v) {
        auto sid = lot.allocateSpot(v);
        if (!sid) { cout << "No spot for " << to_string(v.getSize()) << endl; return nullopt; }
        string tid = newTicketId(*sid, v);
        cout << "Vehicle " << v.getPlate() << " parked at " << *sid << " (Ticket " << tid << ")" << endl;
        notify();
        return tid;
    }

    void checkOut(string ticketId) {
        auto tOpt = store.getTicket(ticketId);
        if (!tOpt) { cout << "Ticket not found!\n"; return; }

        ParkingTicket* t = *tOpt;
        t->exitTime = chrono::system_clock::now();
        t->fee = calc.calculate(*t);
        t->status = TicketStatus::CLOSED;
        lot.freeSpot(t->spotId);
        cout << "Vehicle " << t->plate << " exited. Fee: $" << t->fee << endl;
        notify();
    }

    void showAvailability() { lot.printAvailability(); }
};

// ==================== MAIN ====================
int main() {
    ParkingManager manager;
    ConsoleObserver obs;
    manager.registerObserver(&obs);

    // Create some spots
    manager.addSpot("S1", SpotSize::Small);
    manager.addSpot("M1", SpotSize::Medium);
    manager.addSpot("L1", SpotSize::Large);

    manager.showAvailability();

    // Check-in
    auto car = VehicleFactory::createVehicle("car", "KA01AB1234");
    auto bike = VehicleFactory::createVehicle("bike", "MH12CD9876");

    auto t1 = manager.checkIn(car);
    auto t2 = manager.checkIn(bike);
    manager.showAvailability();

    // Simulate time
    this_thread::sleep_for(chrono::seconds(200));

    if (t1) manager.checkOut(*t1);
    if (t2) manager.checkOut(*t2);
    manager.showAvailability();

    return 0;
}
