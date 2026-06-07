#include <deque>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;
class Symbol {
private:
  char mark;

public:
  Symbol(char mark) { this->mark = mark; }
  char getMark() { return mark; }
};
class Player {
private:
  int id;
  string name;
  Symbol *mark;

public:
  Player(int id, string name, Symbol *symbol) {
    this->id = id;
    this->name = name;
    this->mark = symbol;
  }
  string getName() { return name; }
  Symbol *getSymbol() { return mark; }
};
class Board {
private:
  int size;
  vector<vector<char>> board;

public:
  Board(int n) {
    this->size = n;
    board.resize(n, vector<char>(n, ' '));
  }
  bool iscellEmpty(int r, int c) { return board[r][c] == ' '; }
  void placeSymbol(int r, int c, char ch) { board[r][c] = ch; }
  int getsize() { return size; }
  char getcell(int r, int c) { return board[r][c]; }
  bool isFull() {
    for (auto &row : board)
      for (char c : row)
        if (c == ' ')
          return false;

    return true;
  }

  void display() {
    cout << "\n";

    for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
        cout << board[i][j];

        if (j < size - 1)
          cout << " | ";
      }

      cout << "\n";

      if (i < size - 1)
        cout << "---------\n";
    }
  }
};
class IObserver {
public:
  virtual ~IObserver() = default;
  virtual void update(string msg) = 0;
};
class ConsoleNotifier : public IObserver {

public:
  void update(string msg) override {
    cout << "\n[Notification] " << msg << endl;
  }
};
class Rules {
public:
  virtual ~Rules() = default;
  virtual bool isValidMove(Board &board, int r, int c) = 0;

  virtual bool checkWin(Board &board, char symbol) = 0;

  virtual bool checkDraw(Board &board) = 0;
};
class Standardrule : public Rules {
public:
  bool isValidMove(Board &board, int r, int c) {
    int n = board.getsize();

    return r >= 0 && r < n && c >= 0 && c < n && board.iscellEmpty(r, c);
  }
  bool checkWin(Board &board, char symbol) {
    int n = board.getsize();
    for (int i = 0; i < n; i++) {
      bool flag = true;
      for (int j = 0; j < n; j++) {
        if (board.getcell(i, j) != symbol) {
          flag = false;
          break;
        }
      }
      if (flag) {
        return true;
      }
    }
    for (int j = 0; j < n; j++) {
      bool win = true;

      for (int i = 0; i < n; i++) {
        if (board.getcell(i, j) != symbol) {
          win = false;
          break;
        }
      }

      if (win)
        return true;
    }

    bool diag = true;

    for (int i = 0; i < n; i++) {
      if (board.getcell(i, i) != symbol) {
        diag = false;
        break;
      }
    }

    if (diag)
      return true;

    bool anti = true;

    for (int i = 0; i < n; i++) {
      if (board.getcell(i, n - i - 1) != symbol) {
        anti = false;
        break;
      }
    }

    return anti;
  }
  bool checkDraw(Board &board) { return board.isFull(); }
};
class Game {
private:
  unique_ptr<Board> board;
  deque<Player *> players;
  vector<IObserver *> observers;
  unique_ptr<Rules> rules;
  bool gameOver = false;

public:
  Game(int size) {
    board = make_unique<Board>(size);
    rules = make_unique<Standardrule>();
  }
  void addPlayer(Player *player) { players.push_back(player); }
  void addObserver(IObserver *ob) { observers.push_back(ob); }
  void notify(string msg) {
    for (auto it : observers) {
      it->update(msg);
    }
  }
  void play() {
    while (!gameOver) {
      Player *current = players.front();
      players.pop_front();
      board->display();

      cout << "\n" << current->getName() << " turn\n";

      int r, c;

      cin >> r >> c;
      char mark = current->getSymbol()->getMark();
      if (!rules->isValidMove(*board, r, c)) {
        cout << "Invalid Move\n";
        players.push_front(current);
        continue;
      }
      board->placeSymbol(r, c, mark);
      if (rules->checkWin(*board, mark)) {
        board->display();

        notify(current->getName() + " Wins!");

        gameOver = true;
        break;
      }

      if (rules->checkDraw(*board)) {
        board->display();

        notify("Game Draw");

        gameOver = true;
        break;
      }

      players.push_back(current);
    }
  }
};
enum class GameType { STANDARD };
class GameFactory {
public:
  static unique_ptr<Game> createGame(GameType type, int size) {
    return make_unique<Game>(size);
  }
};
int main() {
  unique_ptr<Game> game = GameFactory::createGame(GameType::STANDARD, 3);
  Player *p1 = new Player(1, "Karthik", new Symbol('X'));
  Player *p2 = new Player(2, "Kohli", new Symbol('O'));
  game->addPlayer(p1);
  game->addPlayer(p2);
  ConsoleNotifier *notifier = new ConsoleNotifier();
  game->addObserver(notifier);

  game->play();
  delete notifier;
  delete p1->getSymbol();
  delete p1;
  delete p2->getSymbol();
  delete p2;

  return 0;
}