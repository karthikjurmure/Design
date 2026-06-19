#include <iostream>
#include <vector>
using namespace std;
enum class ItemType { COKE, PEPSI, JUICE, SODA };
class Item {
private:
  ItemType type;
  int price;

public:
  Item(ItemType type, int price) {
    this->type = type;
    this->price = price;
  }

  ItemType getType() { return type; }

  int getPrice() { return price; }
};
enum class Coin { ONE = 1, TWO = 2, FIVE = 5, TEN = 10 };
class ItemShelf {
private:
  int code;

  vector<Item *> items;

public:
  ItemShelf(int code) { this->code = code; }

  void addItem(Item *item) { items.push_back(item); }

  bool isEmpty() { return items.empty(); }

  Item *getItem() {
    if (items.empty())
      return nullptr;

    return items.front();
  }

  void removeItem() {
    if (!items.empty())
      items.erase(items.begin());
  }

  int getCode() { return code; }
};
class Inventory {
private:
  vector<ItemShelf *> shelves;

public:
  void addShelf(ItemShelf *shelf) { shelves.push_back(shelf); }

  ItemShelf *getShelf(int code) {
    for (auto shelf : shelves) {
      if (shelf->getCode() == code)
        return shelf;
    }

    return nullptr;
  }
  vector<ItemShelf *> getShelves() { return shelves; }
};
class VendingMachineState {
public:
  virtual string getStateName() = 0;
  virtual ~VendingMachineState() {};
};
class IdleState : public VendingMachineState {
public:
  string getStateName() override { return "IDLE"; }
};
class HasMoneyState : public VendingMachineState {
public:
  string getStateName() override { return "HasMoneyState"; }
};
class SelectionState : public VendingMachineState {
public:
  string getStateName() override { return "SelectionState"; }
};
class DispenseState : public VendingMachineState {
public:
  string getStateName() override { return "DispenseState"; }
};
class VendingMachine {
private:
  Inventory inventory;
  vector<Coin> insertedCoins;
  VendingMachineState *curstate;
  int selectedCode;

public:
  VendingMachine() {
    curstate = new IdleState();
    selectedCode = -1;
  }
  Inventory &getInventory() { return inventory; }
  void insertCoin(Coin coin) {
    insertedCoins.push_back(coin);
    delete curstate;
    curstate = new HasMoneyState();
    cout << "Inserted Coin : " << static_cast<int>(coin) << endl;
  }
  void selectCode(int code) {
    ItemShelf *shelf = inventory.getShelf(code);
    if (shelf == nullptr) {
      cout << "Invalid Code\n";
      return;
    }
    delete curstate;
    curstate = new SelectionState();
    selectedCode = code;
    cout << "Selected Product Code : " << code << endl;
  }
  int getBalance() {
    int total = 0;

    for (auto coin : insertedCoins) {
      total += static_cast<int>(coin);
    }

    return total;
  }
  void dispenceProduct() {
    if (selectedCode == -1) {
      cout << "No Product Selected\n";
      return;
    }
    ItemShelf *shelf = inventory.getShelf(selectedCode);
    if (shelf == nullptr) {
      cout << "Invalid Shelf\n";
      return;
    }
    if (shelf->isEmpty()) {
      cout << "Out Of Stock\n";
      return;
    }
    Item *item = shelf->getItem();
    int balance = getBalance();
    if (balance < item->getPrice()) {
      cout << "Insufficient Balance\n";

      cout << "Price : " << item->getPrice() << endl;

      cout << "Paid : " << balance << endl;

      return;
    }
    delete curstate;
    curstate = new DispenseState();
    cout << "\nDispensing Product...\n";
    switch (item->getType()) {
    case ItemType::COKE:
      cout << "COKE\n";
      break;
    case ItemType::PEPSI:
      cout << "PEPSI\n";
      break;
    case ItemType::JUICE:
      cout << "JUICE\n";
      break;
    case ItemType::SODA:
      cout << "SODA\n";
      break;
    }
    shelf->removeItem();
    int change = balance - item->getPrice();
    cout << "Change Returned : " << change << endl;

    insertedCoins.clear();

    selectedCode = -1;

    delete curstate;
    curstate = new IdleState();

    cout << "Machine Back To Idle\n";
  }
  void desplayInventory() {
    cout << "\nInventory\n";
    for (auto &shelf : inventory.getShelves()) {
      cout << "Shelf : " << shelf->getCode();

      if (shelf->isEmpty()) {
        cout << " -> Empty\n";
      } else {
        cout << " -> Available\n";
      }
    }
  }
  ~VendingMachine() { delete curstate; }
};
int main() {
  VendingMachine vm;
  ItemShelf *shelf1 = new ItemShelf(101);
  ItemShelf *shelf2 = new ItemShelf(102);
  shelf1->addItem(new Item(ItemType::COKE, 10));
  shelf1->addItem(new Item(ItemType::COKE, 10));
  shelf2->addItem(new Item(ItemType::PEPSI, 20));
  vm.getInventory().addShelf(shelf1);
  vm.getInventory().addShelf(shelf2);
  vm.desplayInventory();
  vm.insertCoin(Coin::TEN);
  vm.insertCoin(Coin::FIVE);
  cout << "\nBalance = " << vm.getBalance() << endl;
  vm.selectCode(101);
  vm.dispenceProduct();
  vm.desplayInventory();
  vm.insertCoin(Coin::TEN);
  vm.insertCoin(Coin::FIVE);
  cout << "\nBalance = " << vm.getBalance() << endl;
  vm.selectCode(101);
  vm.dispenceProduct();
  vm.desplayInventory();
  return 0;
}
