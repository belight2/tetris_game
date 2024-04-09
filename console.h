#ifndef CONSOLE_H
#define CONSOLE_H

#include <string>

namespace console {
// FPS
const int FPS = 60;
// 화면에 출력 가능한 최대 너비
const int SCREEN_WIDTH = 25;
// 화면에 출력 가능한 최대 높이
const int SCREEN_HEIGHT = 25;

// console 라이브러리를 초기화 한다. 게임 시작 전에 무조건 한번만 호출해야 한다.
void init();

// 화면을 갱신하고 60 FPS를 맞추기 위해 다음 프레임 시간까지 대기한다.
// 각 게임 루프의 마지막에 호출해야 한다.
void wait();

// x, y 위치에 문자열 s를 그린다.
void draw(int x, int y, const char *s);

// x, y 위치에 문자열 s를 그린다.
void draw(int x, int y, std::string s);

// ┏━┓
// ┃ ┃
// ┗━┛
// 위 형태의 박스를 그린다. 왼쪽 위는 minX, minY, 오른쪽 아래는 maxX, maxY로
// 지정한다.
void drawBox(int minX, int minY, int maxX, int maxY);

// 문자열 s를 로그로 출력한다.
void log(std::string s);

// 화면에 그려진 것을 전부 지운다.
// 필요에 따라 각 게임 루프의 시작 부분에서 호출 할 수 있다.
void clear();

// K_LEFT: 왼쪽 방향키
// K_RIGHT: 오른쪽 방향키
// K_UP: 위쪽 방향키
// K_DOWN: 아래쪽 방향키
// K_ESC: ESC
// K_ENTER: 엔터
// K_SPACE: 스페이스
// K_Z: Z
// K_X: X
// K_OTHER: 위의 키들 이외의 키
enum Key {
  K_NONE = 0,
  K_OTHER,
  K_LEFT,
  K_RIGHT,
  K_UP,
  K_DOWN,
  K_ESC,
  K_SPACE,
  K_Z,
  K_X,
  K_ENTER
};

// 키가 눌렸는지 확인한다.
bool key(Key k);

} // namespace console

#endif
