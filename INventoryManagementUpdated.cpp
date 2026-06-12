#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;

enum class ProductCategory { CLOTH, GROCERY, ELECTRONIC };
class Product {
private:
  string sku;
  string name;
  ProductCategory category;
  double price;
  int quantity;
  int threshold;

public:
  Product(string sku, string name, ProductCategory category, double price,
          int quantity, int threshold) {
    this->sku = sku;
    this->name = name;
    this->category = category;
    this->price = price;
    this->quantity = quantity;
    this->threshold = threshold;
  }
  string getSku() const { return sku; }

  string getName() const { return name; }

  int getQuantity() const { return quantity; }

  int getThreshold() const { return threshold; }

  void addStock(int qty) { quantity += qty; }

  bool removeStock(int qty) {
    if (qty > quantity)
      return false;

    quantity -= qty;
    return true;
  }
};
class Warehouse {
private:
  int id;
  string name;
  string location;
  unordered_map<string, Product *> products;

public:
  Warehouse(int id, string name, string loc) {
    this->id = id;
    this->name = name;
    this->location = loc;
  }
  void addProduct(Product *p) { products[p->getSku()] = p; }
  void removeProduct(string sku, int qun) {
    if (products.count(sku)) {
      bool success = products[sku]->removeStock(qun);
      if (success) {
        cout << "Removed " << qun << " units\n";
      } else {
        cout << "Insufficient stock\n";
      }
    }
  }
  void displayInventory() {
    cout << "\nInventory of " << name << "\n";

    for (auto &p : products) {
      cout << p.second->getName() << " -> " << p.second->getQuantity() << endl;
    }
  }
  vector<Product *> getProducts() {
    vector<Product *> res;
    for (auto &p : products) {
      res.push_back(p.second);
    }
    return res;
  }
};
class ReplenishMentStrategy {
public:
  virtual void replenish(Product *p) = 0;
  virtual ~ReplenishMentStrategy() {};
};
class JustinTIme : public ReplenishMentStrategy {
public:
  void replenish(Product *product) override {
    cout << "JIT Replenishment for " << product->getName() << endl;
  }
};
class BulkOrderStrategy : public ReplenishMentStrategy {
public:
  void replenish(Product *product) override {
    cout << "Bulk Order Replenishment for " << product->getName() << endl;
  }
};
class Iobserver {
public:
  virtual void update(Product *p) = 0;
  virtual ~Iobserver() {};
};
class SupplierNotifier : public Iobserver {
public:
  void update(Product *p) {
    cout << "Supplier Alert -> " << p->getName() << " stock low" << endl;
  }
};
class InventoryManager {
private:
  vector<Warehouse *> warehouses;
  static InventoryManager *instance;
  ReplenishMentStrategy *strategy;
  vector<Iobserver *> observers;
  InventoryManager() : strategy(nullptr) {}

public:
  static InventoryManager *getInsatnce() {
    if (instance == nullptr) {
      instance = new InventoryManager();
    }
    return instance;
  }
  void setStrategy(ReplenishMentStrategy *r) { this->strategy = r; }
  void addWarehouse(Warehouse *h) { warehouses.push_back(h); }
  void addObserver(Iobserver *o) { observers.push_back(o); }
  void notifyObservers(Product *p) {
    for (auto &it : observers) {
      it->update(p);
    }
  }
  void checkLowStock() {
    cout << "\nLow Stock Products:\n";
    for (auto &w : warehouses) {
      for (auto &p : w->getProducts()) {
        if (p->getQuantity() < p->getThreshold()) {
          cout << p->getName() << " needs replenishment\n";
          if (strategy != nullptr) {
            notifyObservers(p);
            strategy->replenish(p);
          } else {
            cout << "No strategy set for replenishment.\n";
          }
        }
      }
    }
  }
};
InventoryManager *InventoryManager::instance = nullptr;
int main() {
  InventoryManager *im = InventoryManager::getInsatnce();
  im->addObserver(new SupplierNotifier());
  Warehouse *w1 = new Warehouse(1, "Hyderabad", "loc1");
  im->addWarehouse(w1);
  Product *laptop = new Product("LAP123", "Laptop", ProductCategory::ELECTRONIC,
                                50000, 20, 10);

  Product *mobile = new Product("MOB111", "Mobile", ProductCategory::ELECTRONIC,
                                30000, 5, 10);
  w1->addProduct(laptop);
  w1->addProduct(mobile);
  w1->displayInventory();
  im->setStrategy(new JustinTIme());
  im->checkLowStock();
  return 0;
}
