#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

using namespace std;

enum class Color { BLACK, WHITE };
class Position {
public:
  int row;
  int col;
  Position(int r, int c) {
    this->row = r;
    this->col = c;
  }
};
class Piece {
protected:
  Color color;

public:
  Piece(Color c) { this->color = c; }
  virtual bool isValidmove(Position start, Position end) = 0;
  Color getColor() { return color; }
  virtual char getSymbol() = 0;
  virtual ~Piece() {};
};
class King : public Piece {
public:
  King(Color c) : Piece(c) {}
  char getSymbol() override { return color == Color::WHITE ? 'K' : 'k'; }
  bool isValidmove(Position start, Position end) override {
    int rowdif = abs(start.row - end.row);
    int coldif = abs(start.col - end.col);
    return rowdif <= 1 && coldif <= 1;
  }
};
class Queen : public Piece {
public:
  Queen(Color c) : Piece(c) {}

  char getSymbol() override { return color == Color::WHITE ? 'Q' : 'q'; }

  bool isValidmove(Position start, Position end) override {

    int rowDiff = abs(start.row - end.row);
    int colDiff = abs(start.col - end.col);

    return rowDiff == colDiff || start.row == end.row || start.col == end.col;
  }
};
class Rook : public Piece {
public:
  Rook(Color c) : Piece(c) {}

  char getSymbol() override { return color == Color::WHITE ? 'R' : 'r'; }

  bool isValidmove(Position start, Position end) override {
    return start.row == end.row || start.col == end.col;
  }
};
class Bishop : public Piece {
public:
  Bishop(Color c) : Piece(c) {}

  char getSymbol() override { return color == Color::WHITE ? 'B' : 'b'; }

  bool isValidmove(Position start, Position end) override {
    int rowDiff = abs(start.row - end.row);
    int colDiff = abs(start.col - end.col);

    return rowDiff == colDiff;
  }
};
class Knight : public Piece {
public:
  Knight(Color c) : Piece(c) {}

  char getSymbol() override { return color == Color::WHITE ? 'N' : 'n'; }

  bool isValidmove(Position start, Position end) override {
    int rowDiff = abs(start.row - end.row);
    int colDiff = abs(start.col - end.col);
    return (rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2);
  }
};
class Pawn : public Piece {
public:
  Pawn(Color c) : Piece(c) {}

  char getSymbol() override { return color == Color::WHITE ? 'P' : 'p'; }

  bool isValidmove(Position start, Position end) override {
    if (color == Color::WHITE) {
      return end.row == start.row - 1 && end.col == start.col;
    }
    return end.row == start.row + 1 && end.col == start.col;
  }
};
class Cell {
public:
  Position *position;
  shared_ptr<Piece> piece;
  Cell(int r, int c) : position(new Position(r, c)), piece(nullptr) {}
};

class Player {
private:
  string name;
  Color color;

public:
  Player(string name, Color c) {
    this->name = name;
    this->color = c;
  }
  Color getColor() { return color; }
  string getName() { return name; }
};
class Board {
private:
  vector<vector<Cell>> grid;

public:
  Board() : grid(8, vector<Cell>()) {
    for (int r = 0; r < 8; r++) {
      for (int c = 0; c < 8; c++) {
        grid[r].emplace_back(r, c);
      }
    }
    initialize();
  }
  void initialize() {
    grid[7][0].piece = make_shared<Rook>(Color::WHITE);
    grid[7][1].piece = make_shared<Knight>(Color::WHITE);
    grid[7][2].piece = make_shared<Bishop>(Color::WHITE);
    grid[7][3].piece = make_shared<Queen>(Color::WHITE);
    grid[7][4].piece = make_shared<King>(Color::WHITE);
    grid[7][5].piece = make_shared<Bishop>(Color::WHITE);
    grid[7][6].piece = make_shared<Knight>(Color::WHITE);
    grid[7][7].piece = make_shared<Rook>(Color::WHITE);
    for (int c = 0; c < 8; c++) {
      grid[6][c].piece = make_shared<Pawn>(Color::WHITE);
    }
    grid[0][0].piece = make_shared<Rook>(Color::BLACK);
    grid[0][1].piece = make_shared<Knight>(Color::BLACK);
    grid[0][2].piece = make_shared<Bishop>(Color::BLACK);
    grid[0][3].piece = make_shared<Queen>(Color::BLACK);
    grid[0][4].piece = make_shared<King>(Color::BLACK);
    grid[0][5].piece = make_shared<Bishop>(Color::BLACK);
    grid[0][6].piece = make_shared<Knight>(Color::BLACK);
    grid[0][7].piece = make_shared<Rook>(Color::BLACK);
    for (int c = 0; c < 8; c++) {
      grid[1][c].piece = make_shared<Pawn>(Color::BLACK);
    }
  }
  void display() {

    cout << "\nChess Board\n\n";

    for (int r = 0; r < 8; r++) {

      for (int c = 0; c < 8; c++) {

        if (grid[r][c].piece)
          cout << grid[r][c].piece->getSymbol() << " ";
        else
          cout << ". ";
      }

      cout << endl;
    }

    cout << endl;
  }
  bool movePiece(Position start, Position end) {
    auto piece = grid[start.row][start.col].piece;
    if (!piece) {
      cout << "No piece found!\n";
      return false;
    }
    if (!piece->isValidmove(start, end)) {
      cout << "Invalid move!\n";
      return false;
    }
    grid[end.row][end.col].piece = piece;
    grid[start.row][start.col].piece = nullptr;
    return true;
  }
};
class ChessGame {
private:
  Board *board = new Board();
  Player *player1;
  Player *player2;

public:
  ChessGame() {
    player1 = new Player("player1", Color::WHITE);
    player2 = new Player("player2", Color::BLACK);
  }
  void startGame() {
    board->display();

    cout << "Move White Pawn "
            "(6,0 -> 5,0)\n";

    board->movePiece(Position(6, 0), Position(5, 0));

    board->display();
  }
};
int main() {
  ChessGame *game = new ChessGame();
  game->startGame();
  return 0;
}
