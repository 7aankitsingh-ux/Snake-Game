#include <iostream>
#include <conio.h>
#include <windows.h>
using namespace std;
const int W = 30;       
const int H = 15;       
const int MAXLEN = 200; 
class GameException {
public:
  const char *msg;
  GameException(const char *m) : msg(m) {}
};
class WallHit : public GameException {
public:
  WallHit() : GameException("You hit the wall!") {}
};
class SelfBite : public GameException {
public:
  SelfBite() : GameException("You bit yourself!") {}
};
class BoardFull : public GameException {
public:
  BoardFull() : GameException("Maximum length reached - YOU WIN!") {}
};
class BadInput : public GameException {
public:
  BadInput() : GameException("Invalid level! Choose 1, 2 or 3.") {}
};
class QuitGame : public GameException {
public:
  QuitGame() : GameException("Game quit by player.") {}
};

class Snake {
  int x[MAXLEN], y[MAXLEN];
  int len;
  int dir;

public:
  Snake() {
    len = 3;
    dir = 3;
    x[0] = W / 2;
    y[0] = H / 2;
    for (int i = 1; i < len; i++) {
      x[i] = x[0] - i;
      y[i] = y[0];
    }
  }
  void setDir(int d) {
    if ((dir == 0 && d == 1) || (dir == 1 && d == 0) || (dir == 2 && d == 3) ||
        (dir == 3 && d == 2))
      return;
    dir = d;
  }
  int nextX() const { return x[0] + (dir == 3) - (dir == 2); }
  int nextY() const { return y[0] + (dir == 1) - (dir == 0); }
  void move(bool grow) {
    int nx = nextX(), ny = nextY();
    if (grow) {
      if (len >= MAXLEN)
        throw BoardFull();
      len++;
    }
    for (int i = len - 1; i > 0; i--) {
      x[i] = x[i - 1];
      y[i] = y[i - 1];
    }
    x[0] = nx;
    y[0] = ny;
    if (nx <= 0 || nx >= W - 1 || ny <= 0 || ny >= H - 1)
      throw WallHit();
    for (int i = 1; i < len; i++)
      if (x[i] == nx && y[i] == ny)
        throw SelfBite();
  }
  bool occupies(int a, int b) const {
    for (int i = 0; i < len; i++)
      if (x[i] == a && y[i] == b)
        return true;
    return false;
  }
  int headX() const { return x[0]; }
  int headY() const { return y[0]; }
  int length() const { return len; }
};
class Game {
  Snake s;
  int fx, fy, score;
  unsigned int seed;
  int speed; 
  int nextRand(int n) {
    seed = seed * 1103515245u + 12345u;
    return (int)((seed >> 16) % n);
  }
  void placeFood() {
    for (int t = 0; t < 1000; t++) {
      int a = 1 + nextRand(W - 2), b = 1 + nextRand(H - 2);
      if (!s.occupies(a, b)) {
        fx = a;
        fy = b;
        return;
      }
    }
    throw BoardFull();
  }
  void draw() {
    cout << "\033[2J\033[H";
    cout << "SNAKE GAME   Score: " << score << "   Length: " << s.length()
         << "\n";
    for (int r = 0; r < H; r++) {
      for (int c = 0; c < W; c++) {
        if (r == 0 || r == H - 1 || c == 0 || c == W - 1)
          cout << '#';
        else if (r == s.headY() && c == s.headX())
          cout << 'O';
        else if (s.occupies(c, r))
          cout << 'o';
        else if (r == fy && c == fx)
          cout << '*';
        else
          cout << ' ';
      }
      cout << '\n';
    }
    cout << "Arrows/WASD: move   P: pause   Q: quit\n";
  }
  void readKeys() {
    if (!kbhit())
      return;
    int ch = getch();
    if (ch == 0 || ch == 224) {
      ch = getch();
      if (ch == 72)
        s.setDir(0);
      else if (ch == 80)
        s.setDir(1);
      else if (ch == 75)
        s.setDir(2);
      else if (ch == 77)
        s.setDir(3);
    } else if (ch == 'w' || ch == 'W')
      s.setDir(0);
    else if (ch == 's' || ch == 'S')
      s.setDir(1);
    else if (ch == 'a' || ch == 'A')
      s.setDir(2);
    else if (ch == 'd' || ch == 'D')
      s.setDir(3);
    else if (ch == 'p' || ch == 'P') {
      cout << "PAUSED - press any key";
      getch();
    } else if (ch == 'q' || ch == 'Q')
      throw QuitGame();
  }

public:
  Game(int level) : score(0), seed(1) {
    if (level < 1 || level > 3)
      throw BadInput();
    speed = 50 * (4 - level) + 100;
    fx = fy = 1;
  }
  void run() {
    cout << "Press any key to start...";
    unsigned int t = 1;
    while (!kbhit())
      t++;
    getch();
    seed = t;
    try {
      placeFood();
      while (true) {
        draw();
        int elapsed = 0;
        while (elapsed < speed) {
          readKeys();
          Sleep(10);
          elapsed += 10;
        }
        bool ate = (s.nextX() == fx && s.nextY() == fy);
        s.move(ate);
        if (ate) {
          score += 10;
          if (speed > 80)
            speed -= 20;  
          placeFood();
        }
      }
    } catch (GameException &e) {
      draw();
      cout << "\n" << e.msg << "\nFinal Score: " << score << endl;
    }
  }
};

int main() {
  try {
    int level;
    cout << "===== SNAKE GAME =====\n1. Easy\n2. Medium\n3. Hard\nSelect "
            "level: ";
    cin >> level;
    if (cin.fail())
      throw BadInput();
    Game g(level);
    g.run();
  } catch (GameException &e) {
    cout << "\nError: " << e.msg << endl;
  } catch (...) {
    cout << "\nUnexpected error occurred.\n";
  }
  cout << "Press any key to exit...";
  getch();
  return 0;
}
