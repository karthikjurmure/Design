#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;
enum class VehicleType { LUXURY, ECONOMY, SUV };
enum class VehicleStatus { AVAILABLE, RESERVED, MAINTANANCE, RENTED };
enum class ReservationStatus { PENDING, CONFIRMED, CANCELLED, COMPLETED };
class Vehicle {
protected:
  string registrationNumber;
  string model;
  VehicleType vehicletype;
  VehicleStatus status;
  double rentalPrice;

public:
  Vehicle(string r, string m, VehicleType t, VehicleStatus s, double p) {
    this->registrationNumber = r;
    this->model = m;
    this->vehicletype = t;
    this->status = s;
    this->rentalPrice = p;
  }
  virtual double calcuclateRentalfee(int days) = 0;
  string getRegistrationNumber() { return registrationNumber; }
  string getModel() { return model; }
  VehicleStatus getStatus() { return status; }
  void setStatus(VehicleStatus s) { this->status = s; }
  double getBaseRentalPrice() { return rentalPrice; }
  virtual ~Vehicle() {}
};
class LuxuryVehicle : public Vehicle {
public:
  LuxuryVehicle(string registrationNumber, string model, double price)
      : Vehicle(registrationNumber, model, VehicleType::LUXURY,
                VehicleStatus::AVAILABLE, price) {}
  double calcuclateRentalfee(int days) override { return days * rentalPrice; }
};
class SUVVehicle : public Vehicle {
public:
  SUVVehicle(string regNo, string model, double price)
      : Vehicle(regNo, model, VehicleType::SUV, VehicleStatus::AVAILABLE,
                price) {}

  double calcuclateRentalfee(int days) override {
    return rentalPrice * days * 1.5;
  }
};
class EconomyVehicle : public Vehicle {
public:
  EconomyVehicle(string regNo, string model, double price)
      : Vehicle(regNo, model, VehicleType::ECONOMY, VehicleStatus::AVAILABLE,
                price) {}

  double calcuclateRentalfee(int days) override { return rentalPrice * days; }
};
class VehicleFactory {
public:
  static Vehicle *createVehicle(VehicleType type, string regno, string model,
                                double price) {
    switch (type) {
    case VehicleType::LUXURY:
      return new LuxuryVehicle(regno, model, price);
    case VehicleType::SUV:
      return new SUVVehicle(regno, model, price);
    case VehicleType::ECONOMY:
      return new EconomyVehicle(regno, model, price);
    default:
      return nullptr;
    }
  }
};
class RentalStore {
private:
  int id;
  string name;
  unordered_map<string, Vehicle *> vehicles;

public:
  RentalStore(int id, string name) {
    this->id = id;
    this->name = name;
  }
  void addVehicle(Vehicle *v) { vehicles[v->getRegistrationNumber()] = v; }
  Vehicle *getVehicle(string regno) {
    if (vehicles.count(regno)) {
      return vehicles[regno];
    }
    return nullptr;
  }
};
class User {
private:
  int id;
  string name;
  string email;

public:
  User(int id, string name, string mail) {
    this->id = id;
    this->name = name;
    this->email = mail;
  }
  string getName() { return name; }
  int getId() { return id; }
};
class Reservation {
private:
  int reservationId;
  User *user;
  Vehicle *vehicle;
  int days;
  ReservationStatus status;

public:
  Reservation(int id, User *user, Vehicle *vehicle, int days) {
    this->reservationId = id;
    this->user = user;
    this->vehicle = vehicle;
    this->days = days;
    status = ReservationStatus::PENDING;
  }
  int getId() { return reservationId; }
  double getAmount() { return vehicle->calcuclateRentalfee(days); }
  void confirm() {
    status = ReservationStatus::CONFIRMED;
    vehicle->setStatus(VehicleStatus::RESERVED);
  }
  void complete() {
    status = ReservationStatus::COMPLETED;
    vehicle->setStatus(VehicleStatus::AVAILABLE);
  }
};
class PaymentStrategy {
public:
  virtual void processPayment(double amount) = 0;

  virtual ~PaymentStrategy() {}
};

class CreditCardPayment : public PaymentStrategy {
public:
  void processPayment(double amount) override {
    cout << "Credit Card Payment : " << amount << endl;
  }
};

class CashPayment : public PaymentStrategy {
public:
  void processPayment(double amount) override {
    cout << "Cash Payment : " << amount << endl;
  }
};

class PaypalPayment : public PaymentStrategy {
public:
  void processPayment(double amount) override {
    cout << "PayPal Payment : " << amount << endl;
  }
};
class ReservationManager {
private:
  int nextreservationId = 1;
  unordered_map<int, Reservation *> reservations;

public:
  Reservation *createReservation(User *user, Vehicle *vehicle, int days) {
    Reservation *res =
        new Reservation(nextreservationId++, user, vehicle, days);
    reservations[res->getId()] = res;
    return res;
  }
};
class RentalSystem {
private:
  static RentalSystem *instance;
  vector<RentalStore *> stores;
  ReservationManager reservationmanager;
  unordered_map<int, User *> users;
  RentalSystem() {};

public:
  static RentalSystem *getInstance() {
    if (instance == nullptr) {
      instance = new RentalSystem();
    }
    return instance;
  }
  void addStore(RentalStore *store) { stores.push_back(store); }
  void registerUser(User *user) { users[user->getId()] = user; }
  Reservation *bookVehicle(int userId, Vehicle *vehicle, int days) {
    if (vehicle->getStatus() != VehicleStatus::AVAILABLE) {
      cout << "Vehicle unavailable\n";
      return nullptr;
    }
    vehicle->setStatus(VehicleStatus::RESERVED);
    return reservationmanager.createReservation(users[userId], vehicle, days);
  }
  bool makePayment(Reservation *reservation, PaymentStrategy *strategy) {
    if (reservation == nullptr || strategy == nullptr) {
      return false;
    }
    double amount = reservation->getAmount();
    strategy->processPayment(amount);
    reservation->confirm();
    return true;
  }
};
RentalSystem *RentalSystem::instance = nullptr;
int main() {
  RentalSystem *system = RentalSystem::getInstance();
  User *user = new User(1, "karthik", "karthik@gmail.com");
  User *user2 = new User(2, "ktm", "ktm@gmail.com");
  RentalStore *Hyd = new RentalStore(1, "Hyderbad store");
  system->addStore(Hyd);
  Vehicle *suv = VehicleFactory::createVehicle(VehicleType::LUXURY,
                                               "TS09AB1234", "creta", 2000);
  Hyd->addVehicle(suv);
  system->registerUser(user);
  system->registerUser(user2);
  Reservation *reservation = system->bookVehicle(user->getId(), suv, 10);
  Reservation *reservation2 = system->bookVehicle(user2->getId(), suv, 10);
  PaymentStrategy *payment = new CashPayment();
  system->makePayment(reservation, payment);
  system->makePayment(reservation2, payment);
  cout << "Booking Successful\n";
  return 0;
}
