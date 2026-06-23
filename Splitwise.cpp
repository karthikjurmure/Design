#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

using namespace std;
class User {
public:
  int id;
  string name;
  User(int id, string name) {
    this->id = id;
    this->name = name;
  }
  ~User() {};
};
class Split {
public:
  virtual vector<double> calculatesplit(double amount, int users) = 0;
  virtual ~Split() {};
};
class EqualSplit : public Split {
public:
  vector<double> calculatesplit(double amount, int users) override {
    vector<double> res(users, amount / users);
    return res;
  }
};
class PercentageSplit : public Split {
private:
  vector<double> percentage;

public:
  PercentageSplit(vector<double> p) { this->percentage = p; }
  vector<double> calculatesplit(double amount, int users) override {
    vector<double> res(users);
    for (int i = 0; i < users; i++) {
      res[i] = amount * percentage[i] / 100.0;
    }
    return res;
  }
};
class SplitFactory {
public:
  static Split *createEqualsplit() { return new EqualSplit(); }
  static Split *createPercentagesplit(vector<double> p) {
    return new PercentageSplit(p);
  }
};
class Expences {
public:
  unordered_map<string, double> share;
  string description;
  double amount;
  User *paidby;
  vector<User *> participants;
  Expences(string d, double a, User *u, vector<User *> p) {
    description = d;
    amount = a;
    paidby = u;
    participants = p;
  }
};
class BalanceSheet {
private:
  unordered_map<string, unordered_map<string, double>> balances;

public:
  void updateBalance(User *debtor, User *creditor, double amount) {
    balances[debtor->name][creditor->name] += amount;
  }
  void showBalance() {
    cout << "\n===== BALANCES =====\n";

    for (auto &debtor : balances) {

      for (auto &creditor : debtor.second) {

        cout << debtor.first << " owes " << creditor.first << " : "
             << creditor.second << endl;
      }
    }
  }
};
class SplitWise {
private:
  BalanceSheet balancesheet;

public:
  void addExpences(Expences *expence, Split *splittype) {
    vector<double> share;
    share = splittype->calculatesplit(expence->amount,
                                      expence->participants.size());
    for (int i = 0; i < expence->participants.size(); i++) {
      User *user = expence->participants[i];
      expence->share[user->name] = share[i];
    }
    User *paidby = expence->paidby;
    for (auto user : expence->participants) {
      if (user == paidby) {
        continue;
      }
      balancesheet.updateBalance(user, paidby, expence->share[user->name]);
    }
  }
  void showBalance() { balancesheet.showBalance(); }
};
int main() {
  User *akash = new User(1, "Akash");
  User *rahul = new User(2, "Rahul");
  User *priya = new User(3, "Priya");

  vector<User *> users = {akash, rahul, priya};
  Expences *dinner = new Expences("Dinner", 600.0, akash, users);
  Split *split = SplitFactory::createEqualsplit();
  Split *splitp = SplitFactory::createPercentagesplit({0, 40.0, 60.0});
  SplitWise *splitwise = new SplitWise();
  //   splitwise->addExpences(dinner, split);
  //   splitwise->showBalance();
  splitwise->addExpences(dinner, splitp);
  splitwise->showBalance();
  return 0;
}
