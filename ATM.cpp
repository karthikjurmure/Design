#include <cstddef>
#include <iostream>
#include <unordered_map>
using namespace std;
class Card {
private:
  int pin;
  string AccNumber;
  string cardNumber;

public:
  Card(int p, string A, string c) {
    this->pin = p;
    this->cardNumber = c;
    this->AccNumber = A;
  }
  bool validatePin(int number) { return pin == number; }
  string getAccountNumber() { return AccNumber; }
};
class Account {
private:
  string AccNumber;
  double balance;

public:
  Account(string num, double b) {
    this->AccNumber = num;
    this->balance = b;
  }
  bool windraw(double amount) {
    if (balance < amount) {
      return false;
    }
    balance -= amount;
    return true;
  }
  void deposit(double amount) { balance += amount; }
  double getBalance() { return balance; }
  string getAccountNumber() { return AccNumber; }
};
class ATMInventory {
private:
  int totalamount;

public:
  ATMInventory(int t) { this->totalamount = t; }
  bool hasSufficientamount(double amount) { return totalamount >= amount; }
  void dispenseCash(double amount) { totalamount -= amount; }
  int getRemainingCash() { return totalamount; }
};
class ATM {
private:
  Card *card;
  ATMInventory inventory;
  unordered_map<string, Account *> accounts;

public:
  ATM(int cash) : inventory(cash) { card = nullptr; }
  void addAcoount(Account *acc) { accounts[acc->getAccountNumber()] = acc; }
  void insertCard(Card *c) {
    if (card != nullptr) {
      cout << "Card already inserted\n";
      return;
    }
    card = c;
    cout << "Card inserted Successfully\n";
  }
  bool Authenticate(int pin) {
    if (card == nullptr) {
      cout << "insert card first\n";
      return false;
    }
    if (card->validatePin(pin)) {
      cout << "PIN Verified Successfully\n";
      return true;
    }
    cout << "Invalid PIN\n";
    return false;
  }
  void checkBalance() {

    if (card == nullptr) {
      cout << "Insert card first\n";
      return;
    }

    string accNo = card->getAccountNumber();

    if (accounts.find(accNo) == accounts.end()) {
      cout << "Account not found\n";
      return;
    }
    Account *account = accounts[accNo];

    cout << "Current Balance : " << account->getBalance() << endl;
  }
  void windrawCash(double amount) {
    if (card == nullptr) {
      cout << "Insert card first\n";
      return;
    }

    string accNo = card->getAccountNumber();
    if (accounts.find(accNo) == accounts.end()) {
      cout << "Account not found\n";
      return;
    }
    Account *account = accounts[accNo];
    if (!account->windraw(amount)) {
      cout << "Insufficient Account Balance\n";
      return;
    }
    if (!inventory.hasSufficientamount(amount)) {
      account->deposit(amount);

      cout << "ATM does not have enough cash\n";
      return;
    }
    inventory.dispenseCash(amount);
    cout << "Please collect cash : " << amount << endl;
  }
  void returnCard() {

    if (card == nullptr) {

      cout << "No card inserted\n";
      return;
    }

    card = nullptr;

    cout << "Card Returned Successfully\n";
  }

  void showATMCash() {

    cout << "ATM Cash Remaining : " << inventory.getRemainingCash() << endl;
  }
};
int main() {
  ATM atm(100000);
  Account *acc1 = new Account("123", 50000);
  Account *acc2 = new Account("102", 8000);
  atm.addAcoount(acc1);
  atm.addAcoount(acc2);
  Card *card1 = new Card(1234, "123", "CARD101");

  cout << "\n===== ATM SESSION =====\n";
  atm.insertCard(card1);
  if (atm.Authenticate(1234)) {
    atm.checkBalance();
    atm.windrawCash(10000);
    atm.checkBalance();
    atm.showATMCash();
  }
  atm.returnCard();
  delete card1;
  delete acc1;
  delete acc2;
  return 0;
}
