#include <climits>
#include <initializer_list>
#include <iostream>
#include <queue>
#include <string>
#include <vector>

using namespace std;
enum class Direction { UP, DOWN, IDLE };
enum class ElevatorState { IDLE, MOVING };
class Request {
public:
  int floor;
  bool internal;
  Direction direction;
  Request(int f, bool i, Direction d) {
    this->floor = f;
    this->internal = i;
    this->direction = d;
  }
};
class Elevator {
public:
  int id;
  int curfloor;
  Direction direction;
  queue<Request> req;
  ElevatorState state;
  Elevator(int id) {
    this->id = id;
    curfloor = 0;
    state = ElevatorState::IDLE;
    direction = Direction::IDLE;
  }
  int getId() { return id; }
  int getcurfloor() { return curfloor; }
  void addRequest(Request r) { req.push(r); }
  void process() {
    while (!req.empty()) {
      auto request = req.front();
      req.pop();
      int destination = request.floor;
      if (destination > curfloor) {
        direction = Direction::UP;
      } else if (destination < curfloor) {
        direction = Direction::DOWN;
      } else {
        direction = Direction::IDLE;
      }
      while (curfloor != destination) {
        if (direction == Direction::UP)
          curfloor++;
        else
          curfloor--;

        cout << "Elevator " << id << " -> " << curfloor << endl;
      }
      cout << "Elevator " << id << " Stopped at " << curfloor << endl;
    }
    direction = Direction::IDLE;
  }
};
class ScheduleStrategy {
public:
  virtual Elevator *SelectElevator(vector<Elevator *> &elevators, int floor,
                                   Direction direction) = 0;
};
class NearestElevatorStrategy : public ScheduleStrategy {
public:
  Elevator *SelectElevator(vector<Elevator *> &elevators, int floor,
                           Direction direction) override {
    Elevator *response = nullptr;
    int mini = INT_MAX;
    for (auto &it : elevators) {
      if (abs(it->getcurfloor() - floor) < mini) {
        mini = abs(it->getcurfloor() - floor);
        response = it;
      }
    }
    return response;
  }
};
class ElevatorController {
private:
  vector<Elevator *> elevators;
  ScheduleStrategy *strategy;

public:
  ElevatorController(int count) {
    for (int i = 1; i <= count; i++) {
      elevators.push_back(new Elevator(i));
    }
    strategy = new NearestElevatorStrategy();
  }
  void requestElevator(int floor, Direction direction) {
    Elevator *ele = strategy->SelectElevator(elevators, floor, direction);
    ele->addRequest(Request(floor, false, direction));
  }
  void run() {
    for (auto ele : elevators) {
      ele->process();
    }
  }
  ~ElevatorController() {
    for (auto ele : elevators) {
      delete ele;
    }
    delete strategy;
  }
};
int main() {
  ElevatorController controller(3);
  controller.requestElevator(5, Direction::UP);
  controller.run();

  controller.requestElevator(9, Direction::DOWN);
  controller.run();

  controller.requestElevator(3, Direction::DOWN);
  controller.run();
  return 0;
}
