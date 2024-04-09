#define IS_UNIX() defined(__APPLE__) || defined(__linux__)
#define IS_WINDOWS() defined(_WIN32)

#if IS_WINDOWS()
#include <Windows.h>
#include <conio.h>
#endif

#if IS_UNIX()
#include <ncurses.h>
#include <unistd.h>
#endif

#include "console.h"
#include <chrono>
#include <cstdio>
#include <deque>
#include <vector>

#define BOX_VERTICAL_STRING "┃"
#define BOX_HORIZONTAL_STRING "━"
#define BOX_RIGHT_TOP_STRING "┓"
#define BOX_LEFT_TOP_STRING "┏"
#define BOX_RIGHT_BOTTOM_STRING "┛"
#define BOX_LEFT_BOTTOM_STRING "┗"

#define LOG_LENGTH 50
#define LOG_COUNT 20

namespace console {

template <typename T> using Vec = std::vector<T>;
using String = std::string;

// Cross-platform codes

#if IS_WINDOWS()
void sleep(int milliseconds) { Sleep(milliseconds); }

void setCursorPosition(int x, int y) {
  COORD coord = {(short)x, (short)y};
  SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

Key key() {
  if (!_kbhit())
    return K_NONE;

  int c = _getch();

  // 방향키
  if (c == 224) {
    c = _getch();
    switch (c) {
    case 75:
      return K_LEFT;
    case 77:
      return K_RIGHT;
    case 72:
      return K_UP;
    case 80:
      return K_DOWN;
    }
  }

  switch (c) {
  case 122:
    return K_Z;
  case 120:
    return K_X;
  case 32:
    return K_SPACE;
  case 27:
    return K_ESC;
  case 13:
    return K_ENTER;
  }

  return K_OTHER;
}

void cls() { system("cls"); }

void setCodePage() {
  system("chcp 65001"); // UTF-8
  system("cls");
}

#endif
#if IS_UNIX()
void sleep(int milliseconds) { usleep(milliseconds * 1000); }

void setCursorPosition(int x, int y) { printf("\033[%d;%dH", y + 1, x + 1); }

int kbhit(void) {
  int ch = getch();

  if (ch != ERR) {
    ungetch(ch);
    return 1;
  } else {
    return 0;
  }
}

Key key() {
  if (!kbhit())
    return K_NONE;

  int c = getch();

  // 방향키
  if (c == 27) {
    c = getch();
    if (c == 91) {
      c = getch();
      switch (c) {
      case 68:
        return K_LEFT;
      case 67:
        return K_RIGHT;
      case 65:
        return K_UP;
      case 66:
        return K_DOWN;
      }
    } else if (c == ERR) {
      return K_ESC;
    }
  }

  switch (c) {
  case 10:
    return K_ENTER;
  case 32:
    return K_SPACE;
  case 122:
    return K_Z;
  case 120:
    return K_X;
  }

  return K_OTHER;
}

void setCodePage() {}

void cls() {
  printf("\033[2J");
  printf("\033[1;1H");
}

#endif

Vec<Key> pressed;

Vec<Vec<String>> screen;
Vec<Vec<String>> previousScreen;

bool logsUpdated;
std::deque<String> logs;

int frame = 0;
std::chrono::time_point<std::chrono::system_clock> startTime;

void init() {
  setCodePage();
  cls();

#if IS_UNIX()
  initscr();
  cbreak();
  noecho();
  nodelay(stdscr, TRUE);
  scrollok(stdscr, TRUE);
#endif

  screen = Vec<Vec<String>>(SCREEN_WIDTH, Vec<String>(SCREEN_HEIGHT, " "));
  previousScreen = screen;
  logs = std::deque<String>(LOG_COUNT);
}

void log(std::string s) {
  auto now = std::chrono::system_clock::now();
  // format HH:MM:SS
  auto now_c = std::chrono::system_clock::to_time_t(now);
  auto time = std::localtime(&now_c);
  char timeString[9];
  std::strftime(timeString, 9, "%T", time);
  auto totalString = std::string(timeString) + ": " + s;
  if (totalString.size() > LOG_LENGTH) {
    totalString = totalString.substr(0, LOG_LENGTH - 3);
    totalString += "...";
  }
  logs.push_front(totalString);
  logsUpdated = true;
  if (logs.size() > LOG_COUNT) {
    logs.pop_back();
  }
}

void wait() {
  if (logsUpdated) {
    logsUpdated = false;
    // clear log screen
    for (int i = 0; i < LOG_LENGTH; i++) {
      for (int j = 0; j < LOG_COUNT; j++) {
        setCursorPosition(SCREEN_WIDTH + 2 + i, j + 1);
        printf(" ");
      }
    }

    for (size_t i = 0; i < LOG_COUNT; i++) {
      setCursorPosition(SCREEN_WIDTH + 2, i + 1);
      printf("%s", logs[i].c_str());
    }
  }

  pressed.clear();
  for (int i = 0; i < SCREEN_WIDTH; i++) {
    for (int j = 0; j < SCREEN_HEIGHT; j++) {
      if (previousScreen[i][j] != screen[i][j]) {
        setCursorPosition(i, j);
        printf("%s", screen[i][j].c_str());
      }
    }
  }
  setCursorPosition(SCREEN_WIDTH, SCREEN_HEIGHT);
  fflush(stdout);

  previousScreen = screen;

  // sleep until next frame
  if (startTime.time_since_epoch().count() == 0)
    startTime = std::chrono::system_clock::now();
  frame += 1;
  auto nextFrame = startTime + std::chrono::milliseconds(frame * 1000 / FPS);
  auto sleepTime = nextFrame - std::chrono::system_clock::now();
  if (sleepTime.count() > 0) {
    console::sleep(
        std::chrono::duration_cast<std::chrono::milliseconds>(sleepTime)
            .count());
  }
}

size_t countUtf8CodePoint(const char *s) {
  size_t i = 0;
  size_t j = 0;
  while (s[i]) {
    if ((s[i] & 0xC0) != 0x80)
      j++;
    i++;
  }
  return j;
}

int getUTF8CodePoint(const char *s, char *buffer) {
  if (s[0] == '\0')
    return 0;

  int numBytes = 0;

  if ((s[0] & 0x80) == 0)
    numBytes = 1;
  if ((s[0] & 0xE0) == 0xC0)
    numBytes = 2;
  else if ((s[0] & 0xF0) == 0xE0)
    numBytes = 3;

  for (int i = 0; i < numBytes; i++)
    buffer[i] = s[i];

  buffer[numBytes] = '\0';

  return numBytes;
}

char *getUTF8CodePoint(const char *s, int index) {
  static char buffer[5];
  int skip = 0;
  for (int i = 0; i <= index; i++) {
    auto bytes = getUTF8CodePoint(s + skip, buffer);
    if (bytes == 0)
      return nullptr;
    skip += bytes;
  }
  return buffer;
}

void draw(int x, int y, const char *s, int index) {
  char *c = getUTF8CodePoint(s, index);

  if (x < 0 || y < 0 || x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT) {
    return;
  }

  if (screen[x][y] == c)
    return;

  screen[x][y] = c;
}

void draw(int x, int y, const char *s) {
  for (size_t i = 0; i < countUtf8CodePoint(s); i++) {
    draw(x + i, y, s, i);
  }
}

void draw(int x, int y, String s) { draw(x, y, s.c_str()); }

void drawBox(int minX, int minY, int maxX, int maxY) {
  draw(minX, minY, BOX_LEFT_TOP_STRING);
  draw(maxX, minY, BOX_RIGHT_TOP_STRING);
  draw(minX, maxY, BOX_LEFT_BOTTOM_STRING);
  draw(maxX, maxY, BOX_RIGHT_BOTTOM_STRING);

  for (int i = minX + 1; i < maxX; i++) {
    draw(i, minY, BOX_HORIZONTAL_STRING);
    draw(i, maxY, BOX_HORIZONTAL_STRING);
  }

  for (int i = minY + 1; i < maxY; i++) {
    draw(minX, i, BOX_VERTICAL_STRING);
    draw(maxX, i, BOX_VERTICAL_STRING);
  }
}

void clear() {
  for (int i = 0; i < SCREEN_WIDTH; i++) {
    for (int j = 0; j < SCREEN_HEIGHT; j++) {
      draw(i, j, " ");
    }
  }
}

void updatePressed() {
  Key k = key();
  while (k != K_NONE) {
    pressed.push_back(k);
    k = key();
  }
}

bool key(Key k) {
  updatePressed();
  for (auto key : pressed)
    if (key == k)
      return true;
  return false;
}

} // namespace console
