#include <cmath>
#include <ctime>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

// ---------------------------------------------------------------- Vehicles
enum class VehicleType { CAR, BIKE, TRUCK };

class Vehicle {
private:
  string licenceNum;
  VehicleType type;

protected:
  Vehicle(const string &licenceNum, VehicleType type)
      : licenceNum(licenceNum), type(type) {}

public:
  virtual ~Vehicle() = default;
  VehicleType getType() const { return type; }
  const string &getLicenceNum() const { return licenceNum; }
};

class Bike : public Vehicle {
public:
  explicit Bike(const string &num) : Vehicle(num, VehicleType::BIKE) {}
};

class Car : public Vehicle {
public:
  explicit Car(const string &num) : Vehicle(num, VehicleType::CAR) {}
};

class Truck : public Vehicle {
public:
  explicit Truck(const string &num) : Vehicle(num, VehicleType::TRUCK) {}
};

class VehicleFactory {
public:
  static unique_ptr<Vehicle> createVehicle(VehicleType type,
                                           const string &number) {
    switch (type) {
    case VehicleType::BIKE:
      return make_unique<Bike>(number);
    case VehicleType::CAR:
      return make_unique<Car>(number);
    case VehicleType::TRUCK:
      return make_unique<Truck>(number);
    }
    throw invalid_argument("Unknown vehicle type");
  }
};

// ---------------------------------------------------------------- Payment
class PaymentStrategy {
public:
  virtual ~PaymentStrategy() = default;
  virtual void pay(double amount) = 0;
};

class Upi : public PaymentStrategy {
public:
  void pay(double amount) override {
    cout << "UPI Payment: Rs. " << amount << endl;
  }
};

class Card : public PaymentStrategy {
public:
  void pay(double amount) override {
    cout << "Card Payment: Rs. " << amount << endl;
  }
};

// ---------------------------------------------------------------- Pricing
class PricingStrategy {
public:
  virtual ~PricingStrategy() = default;
  virtual double calculateFare(double hours) const = 0;
};

class HourlyPricing : public PricingStrategy {
private:
  double ratePerHour;

public:
  explicit HourlyPricing(double rate) : ratePerHour(rate) {}
  double calculateFare(double hours) const override {
    return hours * ratePerHour;
  }
};

// ---------------------------------------------------------------- Slot
class ParkingSlot {
private:
  int slotId;
  VehicleType supportedType;
  bool occupied;

public:
  ParkingSlot(int id, VehicleType type)
      : slotId(id), supportedType(type), occupied(false) {}

  int getSlotId() const { return slotId; }
  VehicleType getSupportedType() const { return supportedType; }
  bool isOccupied() const { return occupied; }

  bool canPark(VehicleType type) const {
    return !occupied && supportedType == type;
  }
  void park() { occupied = true; }
  void unpark() { occupied = false; }
};

// ---------------------------------------------------------------- Ticket
class ParkingTicket {
private:
  int ticketNum;
  string vehicleNum;
  VehicleType vehicleType;
  time_t entryTime;
  int slotId;

public:
  ParkingTicket(int ticketNum, const string &vehicleNum, VehicleType type,
                time_t entryTime, int slotId)
      : ticketNum(ticketNum), vehicleNum(vehicleNum), vehicleType(type),
        entryTime(entryTime), slotId(slotId) {}

  int getTicketNum() const { return ticketNum; }
  const string &getVehicleNum() const { return vehicleNum; }
  VehicleType getVehicleType() const { return vehicleType; }
  time_t getEntryTime() const { return entryTime; }
  int getSlotId() const { return slotId; }
};

// ---------------------------------------------------------------- Lot
class ParkingLot {
private:
  vector<ParkingSlot> slots;
  unordered_map<int, ParkingTicket> activeTickets;
  unordered_map<VehicleType, unique_ptr<PricingStrategy>> pricing;
  int ticketCounter = 1;

  ParkingLot() {
    pricing[VehicleType::BIKE] = make_unique<HourlyPricing>(10);
    pricing[VehicleType::CAR] = make_unique<HourlyPricing>(20);
    pricing[VehicleType::TRUCK] = make_unique<HourlyPricing>(40);
  }

  ParkingSlot *findSlot(int slotId) {
    for (auto &slot : slots) {
      if (slot.getSlotId() == slotId)
        return &slot;
    }
    return nullptr;
  }

public:
  ParkingLot(const ParkingLot &) = delete;
  ParkingLot &operator=(const ParkingLot &) = delete;

  static ParkingLot &getInstance() {
    static ParkingLot instance;
    return instance;
  }

  void addParkingSlot(VehicleType type, int id) {
    if (findSlot(id) != nullptr)
      throw invalid_argument("Slot id already exists");
    slots.emplace_back(id, type);
  }

  ParkingTicket parkVehicle(const Vehicle &vehicle) {
    for (auto &slot : slots) { // reference: the real slot is updated
      if (slot.canPark(vehicle.getType())) {
        slot.park();
        ParkingTicket ticket(ticketCounter++, vehicle.getLicenceNum(),
                             vehicle.getType(), time(nullptr),
                             slot.getSlotId());
        activeTickets.emplace(ticket.getTicketNum(), ticket);
        cout << "Vehicle " << vehicle.getLicenceNum() << " parked at slot "
             << slot.getSlotId() << " (ticket " << ticket.getTicketNum()
             << ")" << endl;
        return ticket;
      }
    }
    throw runtime_error("No slot available");
  }

  // Billed per started hour, minimum one hour.
  double calculateFare(const ParkingTicket &ticket, time_t exitTime) const {
    double hours = ceil(difftime(exitTime, ticket.getEntryTime()) / 3600.0);
    if (hours < 1)
      hours = 1;
    return pricing.at(ticket.getVehicleType())->calculateFare(hours);
  }

  void exitVehicle(int ticketNum, PaymentStrategy &payment,
                   time_t exitTime = time(nullptr)) {
    auto it = activeTickets.find(ticketNum);
    if (it == activeTickets.end())
      throw runtime_error("Invalid or already used ticket");

    const ParkingTicket &ticket = it->second;
    double fare = calculateFare(ticket, exitTime);
    payment.pay(fare);

    if (ParkingSlot *slot = findSlot(ticket.getSlotId()))
      slot->unpark();
    activeTickets.erase(it);
    cout << "Vehicle exited.\n";
  }
};

// ---------------------------------------------------------------- main
int main() {
  ParkingLot &lot = ParkingLot::getInstance();
  lot.addParkingSlot(VehicleType::CAR, 1);
  lot.addParkingSlot(VehicleType::TRUCK, 2);
  lot.addParkingSlot(VehicleType::BIKE, 3);

  auto truck = VehicleFactory::createVehicle(VehicleType::TRUCK, "12345566");
  auto truck2 = VehicleFactory::createVehicle(VehicleType::TRUCK, "99999999");
  Upi upi;

  ParkingTicket t1 = lot.parkVehicle(*truck);

  try {
    lot.parkVehicle(*truck2); // only one truck slot -> must fail
  } catch (const exception &e) {
    cout << "Error: " << e.what() << endl;
  }

  // Simulate leaving after 2.5 hours -> billed for 3 hours -> 3 * 40 = 120
  lot.exitVehicle(t1.getTicketNum(), upi, t1.getEntryTime() + 9000);

  try {
    lot.exitVehicle(t1.getTicketNum(), upi); // ticket already used
  } catch (const exception &e) {
    cout << "Error: " << e.what() << endl;
  }

  ParkingTicket t2 = lot.parkVehicle(*truck2); // slot is free again
  Card card;
  lot.exitVehicle(t2.getTicketNum(), card);
  return 0;
}
