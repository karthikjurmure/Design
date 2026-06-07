#include <cstddef>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

using namespace std;
enum class VehicleType { CAR, BIKE, TRUCK };
class Vehicle {
protected:
  string LicenceNum;
  VehicleType type;

public:
  Vehicle() {}
  VehicleType getType() { return type; }
  string getLicenceNum() { return LicenceNum; }
  virtual ~Vehicle() {};
};
class Bike : public Vehicle {
public:
  Bike(string LicenceNum) {
    this->LicenceNum = LicenceNum;
    this->type = VehicleType::BIKE;
  }
};
class Car : public Vehicle {
public:
  Car(string LicenceNum) {
    this->LicenceNum = LicenceNum;
    this->type = VehicleType::CAR;
  }
};
class Truck : public Vehicle {
public:
  Truck(string LicenceNum) {
    this->LicenceNum = LicenceNum;
    this->type = VehicleType::TRUCK;
  }
};
class VehicleFactory {
public:
  Vehicle *createVehicle(VehicleType type, string number) {
    switch (type) {
    case VehicleType::BIKE:
      return new Bike(number);
    case VehicleType::CAR:
      return new Car(number);
    case VehicleType::TRUCK:
      return new Truck(number);
    default:
      return nullptr;
    }
  }
};
class PaymentStrategy {
public:
  virtual void pay(double amount) = 0;
  virtual ~PaymentStrategy() {};
};
class Upi : public PaymentStrategy {
public:
  void pay(double amount) override {
    cout << "Upi Payment: ₹" << amount << endl;
  }
};
class Card : public PaymentStrategy {
public:
  void pay(double amount) override {
    cout << "Card Payment: ₹" << amount << endl;
  }
};
class ParkingSlot {
private:
  int slotid;
  VehicleType supportedType;
  bool occupied;

public:
  ParkingSlot(int id, VehicleType type, bool occupied) {
    this->slotid = id;
    this->supportedType = type;
    this->occupied = occupied;
  }
  bool isOccupied() { return occupied; }
  bool canPark(VehicleType type) {
    if (supportedType == type && !occupied) {
      return true;
    }
    return false;
  }
  void park() { occupied = true; }
  void unpark() { occupied = false; }
  int getSlotid() { return slotid; }
  VehicleType getSupportedtype() { return supportedType; }
};
class ParkingTicket {
private:
  int ticketNum;
  string VehicleNum;
  time_t entrytime;
  int slotid;

public:
  ParkingTicket(int ticketNum, string VehicleNum, time_t entrytime,
                int slotid) {
    this->ticketNum = ticketNum;
    this->VehicleNum = VehicleNum;
    this->entrytime = entrytime;
    this->slotid = slotid;
  }
  int getslotid() { return slotid; }
  int getticket() { return ticketNum; }
  string getVehicleNum() { return VehicleNum; }
  time_t getEntrytime() { return entrytime; }
};
class ParkingLot {
private:
  vector<ParkingSlot> slots;
  unordered_map<int, ParkingTicket> activeTickets;
  int ticketCounter = 1;
  ParkingLot() {};
  static ParkingLot *instance;

public:
  static ParkingLot *getInstance() {
    if (instance == nullptr) {
      instance = new ParkingLot();
    }
    return instance;
  }
  void addParkingSlot(VehicleType type, int id) {
    slots.emplace_back(id, type, false);
  }
  ParkingTicket parkVehicle(Vehicle vehicle) {
    for (auto it : slots) {
      if (it.canPark(vehicle.getType())) {
        it.park();
        ParkingTicket ticket(ticketCounter++, vehicle.getLicenceNum(),
                             time(NULL), it.getSlotid());
        activeTickets.emplace(ticket.getticket(), ticket);
        cout << "Vehicle parked at slot " << it.getSlotid() << endl;
        return ticket;
      }
    }
    throw runtime_error("No slot available");
  }
  double calculateFare(ParkingTicket *ticket, VehicleType type) {
    time_t exitTime = time(nullptr);

    double hours = difftime(exitTime, ticket->getEntrytime()) / 3600.0;

    if (hours < 1)
      hours = 1;

    switch (type) {
    case VehicleType::BIKE:
      return hours * 10;

    case VehicleType::CAR:
      return hours * 20;

    case VehicleType::TRUCK:
      return hours * 40;
    }

    return 0;
  }
  void exitVehicle(ParkingTicket *ticket, VehicleType type,
                   PaymentStrategy *strategy) {
    double fare = calculateFare(ticket, type);
    strategy->pay(fare);
    for (auto it : slots) {
      if (it.getSlotid() == ticket->getslotid()) {
        it.unpark();
        break;
      }
    }
    activeTickets.erase(ticket->getticket());
    cout << "Vehicle exited.\n";
  }
};
ParkingLot *ParkingLot::instance = nullptr;
int main() {
  ParkingLot *lot = ParkingLot::getInstance();
  lot->addParkingSlot(VehicleType::CAR, 1);
  lot->addParkingSlot(VehicleType::TRUCK, 2);
  lot->addParkingSlot(VehicleType::BIKE, 3);
  VehicleFactory *vehicle = new VehicleFactory;
  Vehicle *v = vehicle->createVehicle(VehicleType::TRUCK, "12345566");
  ParkingTicket ticket = lot->parkVehicle(*v);
  PaymentStrategy *payment = new Upi();

  lot->exitVehicle(&ticket, VehicleType::TRUCK, payment);

  delete payment;
  delete v;
  delete vehicle;

  return 0;
}