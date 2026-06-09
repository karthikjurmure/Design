#include <deque>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

using namespace std;
enum class Direction { UP, DOWN, LEFT, RIGHT };
class Position {
public:
  int row;
  int col;
  Position(int r, int c) {
    this->row = r;
    this->col = c;
  }
};
class SnakeGame {
  int rows;
  int cols;
  deque<Position> snake;
  unordered_set<string> occupied;
  vector<Position> foods;
  int foodIdx;
  string getKey(int r, int c) { return to_string(r) + "#" + to_string(c); }

public:
  SnakeGame(int width, int height, vector<Position> &food) {
    this->rows = height;
    this->cols = width;
    this->foods = food;
    foodIdx = 0;
    snake.push_front(Position(0, 0));
    occupied.insert(getKey(0, 0));
  }
  int Move(Direction direction) {
    Position head = snake.front();
    int newr = head.row;
    int newc = head.col;
    switch (direction) {
    case Direction::UP:
      newr--;
      break;
    case Direction::RIGHT:
      newc++;
      break;
    case Direction::DOWN:
      newr++;
      break;
    case Direction::LEFT:
      newc--;
      break;
    }
    if (newr < 0 || newr >= rows || newc < 0 || newc >= cols) {
      return -1;
    }
    Position tail = snake.back();
    bool eatFood;
    if (foodIdx < foods.size() && newr == foods[foodIdx].row &&
        newc == foods[foodIdx].col) {
      eatFood = true;
      foodIdx++;
    }
    if (!eatFood) {
      occupied.erase(getKey(tail.row, tail.col));
    }
    if (occupied.count(getKey(newr, newc))) {
      return -1;
    }
    snake.push_front(Position(newr, newc));
    occupied.insert(getKey(newr, newc));
    if (!eatFood) {
      snake.pop_back();
    }
    return snake.size() - 1;
  }
  void printSnake() {
    cout << "Snake : ";
    for (auto &cell : snake) {
      cout << "(" << cell.row << "," << cell.col << ") ";
    }
    cout << "\n";
  }
  int getScore() { return snake.size() - 1; }
};
int main() {
  vector<Position> food = {{1, 2}, {0, 1}, {2, 2}};
  SnakeGame game(4, 4, food);
  game.printSnake();
  cout << game.Move(Direction::RIGHT) << endl;
  cout << game.Move(Direction::RIGHT) << endl;
  cout << game.Move(Direction::DOWN) << endl;
  cout << game.Move(Direction::LEFT) << endl;
  game.printSnake();
  cout << "Score:" << game.getScore() << endl;
  return 0;
}
