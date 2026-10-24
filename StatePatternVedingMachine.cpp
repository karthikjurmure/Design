#include <iostream>
#include <string>
using namespace std;
class VendingMachine;
class VendingState {
public:
	virtual VendingState* insertCoin(VendingMachine* m, int coin) = 0;
	virtual VendingState* selectItem(VendingMachine* m) = 0;
	virtual VendingState* dispense(VendingMachine* m) = 0;
	virtual VendingState* returnCoin(VendingMachine* m) = 0;
	virtual VendingState* refill(VendingMachine* m, int qty) = 0;
	virtual string name() = 0;
	virtual ~VendingState() {}
};
class VendingMachine{
public:
    int items;
    int price;
    int balance;
    VendingState* noCoin;
    VendingState* hasCoin;
    VendingState* dispensing;
    VendingState* soldOut;
    VendingState* current;
    VendingMachine(int items, int price); 
    ~VendingMachine() {
        delete noCoin; delete hasCoin; delete dispensing; delete soldOut;
    }
   void insertCoin(int coin) { current = current->insertCoin(this, coin); }
    void selectItem()         { current = current->selectItem(this); }
    void dispense()           { current = current->dispense(this); }
    void returnCoin()         { current = current->returnCoin(this); }
    void refill(int qty)      { current = current->refill(this, qty); }

    void printStatus() {
        cout << "[Items: " << items << " | Balance: Rs " << balance
             << " | State: " << current->name() << "]\n\n";
    }
};
class NoCoinState : public VendingState {
public:
    VendingState* insertCoin(VendingMachine* m, int coin) override {
        m->balance = coin;
        cout << "Coin inserted. Balance: Rs " << m->balance << endl;
        return m->hasCoin;                       // move to HAS_COIN
    }
    VendingState* selectItem(VendingMachine* m) override {
        cout << "Insert coin first!" << endl;
        return this;
    }
    VendingState* dispense(VendingMachine* m) override {
        cout << "Insert coin and select item first!" << endl;
        return this;
    }
    VendingState* returnCoin(VendingMachine* m) override {
        cout << "No coin to return!" << endl;
        return this;
    }
    VendingState* refill(VendingMachine* m, int qty) override {
        m->items += qty;
        cout << "Refilled. Items: " << m->items << endl;
        return this;
    }
    string name() override { return "NO_COIN"; }
};
class HasCoinState : public VendingState {
public:
    VendingState* insertCoin(VendingMachine* m, int coin) override {
        m->balance += coin;
        cout << "More coin added. Balance: Rs " << m->balance << endl;
        return this;
    }
    VendingState* selectItem(VendingMachine* m) override {
        if (m->balance >= m->price) {
            int change = m->balance - m->price;
            if (change > 0) cout << "Change returned: Rs " << change << endl;
            m->balance = 0;
            cout << "Item selected. Dispensing..." << endl;
            return m->dispensing;                // move to DISPENSING
        }
        cout << "Need Rs " << m->price - m->balance << " more." << endl;
        return this;
    }
    VendingState* dispense(VendingMachine* m) override {
        cout << "Select an item first!" << endl;
        return this;
    }
    VendingState* returnCoin(VendingMachine* m) override {
        cout << "Coin returned: Rs " << m->balance << endl;
        m->balance = 0;
        return m->noCoin;                        // back to NO_COIN
    }
    VendingState* refill(VendingMachine* m, int qty) override {
        cout << "Can't refill in this state." << endl;
        return this;
    }
    string name() override { return "HAS_COIN"; }
};
class DispenseState : public VendingState {
public:
    VendingState* insertCoin(VendingMachine* m, int coin) override {
        cout << "Dispensing, please wait. Coin returned: Rs " << coin << endl;
        return this;
    }
    VendingState* selectItem(VendingMachine* m) override {
        cout << "Already dispensing, please wait." << endl;
        return this;
    }
    VendingState* dispense(VendingMachine* m) override {
        m->items--;
        cout << "Item dispensed!" << endl;
        if (m->items > 0) return m->noCoin;      // items left
        cout << "Machine is now sold out!" << endl;
        return m->soldOut;                       // no items left
    }
    VendingState* returnCoin(VendingMachine* m) override {
        cout << "Can't return coin while dispensing." << endl;
        return this;
    }
    VendingState* refill(VendingMachine* m, int qty) override {
        cout << "Can't refill in this state." << endl;
        return this;
    }
    string name() override { return "DISPENSING"; }
};
class SoldOutState : public VendingState {
public:
    VendingState* insertCoin(VendingMachine* m, int coin) override {
        cout << "Sold out. Coin returned: Rs " << coin << endl;
        return this;
    }
    VendingState* selectItem(VendingMachine* m) override {
        cout << "Sold out!" << endl;
        return this;
    }
    VendingState* dispense(VendingMachine* m) override {
        cout << "Sold out!" << endl;
        return this;
    }
    VendingState* returnCoin(VendingMachine* m) override {
        cout << "Sold out. No coin inserted." << endl;
        return this;
    }
    VendingState* refill(VendingMachine* m, int qty) override {
        m->items += qty;
        cout << "Refilled. Items: " << m->items << endl;
        return m->noCoin;                        // back in business
    }
    string name() override { return "SOLD_OUT"; }
};
VendingMachine::VendingMachine(int items, int price) {
    this->items = items;
    this->price = price;
    balance = 0;

    noCoin     = new NoCoinState();
    hasCoin    = new HasCoinState();
    dispensing = new DispenseState();
    soldOut    = new SoldOutState();

    current = (items > 0) ? noCoin : soldOut;
}
int main() {
    VendingMachine machine(2, 20);
    machine.printStatus();

    machine.selectItem();     machine.printStatus();   // no coin yet
    machine.insertCoin(10);   machine.printStatus();   // -> HAS_COIN
    machine.selectItem();     machine.printStatus();   // not enough money
    machine.insertCoin(10);   machine.printStatus();   // balance 20
    machine.selectItem();     machine.printStatus();   // -> DISPENSING
    machine.dispense();       machine.printStatus();   // -> NO_COIN (1 item left)

    machine.insertCoin(20);
    machine.selectItem();
    machine.dispense();       machine.printStatus();   // -> SOLD_OUT

    machine.insertCoin(5);                             // coin rejected
    machine.refill(2);        machine.printStatus();   // -> NO_COIN
    return 0;
}

//////////////////
//////////////////
///////////////
//without state design pattern using simple switch state
#include <iostream>
#include <string>
using namespace std;

enum State { NO_COIN, HAS_COIN, DISPENSING, SOLD_OUT };

class VendingMachine {
private:
    State state;
    int items;
    int price;
    int balance;

public:
    VendingMachine(int items, int price) {
        this->items = items;
        this->price = price;
        balance = 0;
        state = (items > 0) ? NO_COIN : SOLD_OUT;
    }

    void insertCoin(int coin) {
        switch (state) {
            case NO_COIN:
                balance = coin;
                cout << "Coin inserted. Balance: Rs " << balance << endl;
                state = HAS_COIN;
                break;
            case HAS_COIN:
                balance += coin;
                cout << "Additional coin inserted. Balance: Rs " << balance << endl;
                break;
            case DISPENSING:
                cout << "Please wait, dispensing. Coin returned: Rs " << coin << endl;
                break;
            case SOLD_OUT:
                cout << "Sold out. Coin returned: Rs " << coin << endl;
                break;
        }
    }

    void selectItem() {
        switch (state) {
            case NO_COIN:
                cout << "Please insert coin first!" << endl;
                break;
            case HAS_COIN:
                if (balance >= price) {
                    cout << "Item selected. Dispensing..." << endl;
                    if (balance > price)
                        cout << "Change returned: Rs " << balance - price << endl;
                    balance = 0;
                    state = DISPENSING;
                } else {
                    cout << "Insufficient funds. Need Rs " << price - balance << " more." << endl;
                }
                break;
            case DISPENSING:
                cout << "Already dispensing. Please wait." << endl;
                break;
            case SOLD_OUT:
                cout << "Machine is sold out!" << endl;
                break;
        }
    }

    void dispense() {
        switch (state) {
            case NO_COIN:
                cout << "Please insert coin and select item first!" << endl;
                break;
            case HAS_COIN:
                cout << "Please select an item first!" << endl;
                break;
            case DISPENSING:
                cout << "Item dispensed!" << endl;
                items--;
                if (items > 0) {
                    state = NO_COIN;
                } else {
                    cout << "Machine is now sold out!" << endl;
                    state = SOLD_OUT;
                }
                break;
            case SOLD_OUT:
                cout << "Machine is sold out!" << endl;
                break;
        }
    }

    void returnCoin() {
        switch (state) {
            case NO_COIN:
                cout << "No coin to return!" << endl;
                break;
            case HAS_COIN:
                cout << "Coin returned: Rs " << balance << endl;
                balance = 0;
                state = NO_COIN;
                break;
            case DISPENSING:
                cout << "Cannot return coin while dispensing!" << endl;
                break;
            case SOLD_OUT:
                cout << "Sold out. No coin inserted." << endl;
                break;
        }
    }

    void refill(int quantity) {
        if (state == NO_COIN || state == SOLD_OUT) {
            cout << "Items refilling" << endl;
            items += quantity;
            state = NO_COIN;
        } else {
            cout << "Can't refill in this state" << endl;
        }
    }

    void printStatus() {
        string names[] = {"NO_COIN", "HAS_COIN", "DISPENSING", "SOLD_OUT"};
        cout << "\n--- Status ---" << endl;
        cout << "Items remaining: " << items << endl;
        cout << "Inserted coin: Rs " << balance << endl;
        cout << "Current state: " << names[state] << endl << endl;
    }
};

int main() {
    cout << "=== Water Bottle VENDING MACHINE ===" << endl;
    VendingMachine machine(2, 20);
    machine.printStatus();

    cout << "1. Select without coin:" << endl;
    machine.selectItem();
    machine.printStatus();

    cout << "2. Insert coin:" << endl;
    machine.insertCoin(10);
    machine.printStatus();

    cout << "3. Select with insufficient funds:" << endl;
    machine.selectItem();
    machine.printStatus();

    cout << "4. Add more coins:" << endl;
    machine.insertCoin(10);
    machine.printStatus();

    cout << "5. Select item now:" << endl;
    machine.selectItem();
    machine.printStatus();

    cout << "6. Dispense:" << endl;
    machine.dispense();
    machine.printStatus();

    cout << "7. Buy last item:" << endl;
    machine.insertCoin(20);
    machine.selectItem();
    machine.dispense();
    machine.printStatus();

    cout << "8. Use sold out machine:" << endl;
    machine.insertCoin(5);

    cout << "9. Refill:" << endl;
    machine.refill(2);
    machine.printStatus();

    return 0;
}
