
#ifndef GAME_H
#define GAME_H

#include "tetromino.h"
#include <chrono>

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define LINES 40

#define DROP_DELAY 60

class Game {
private:
  // 게임 판을 나타내는 배열
  // board[x][y]가 true 인 경우 x, y 위치에 고정된 블록이 존재하는 것을 의미한다
  bool board_[BOARD_WIDTH][BOARD_HEIGHT];

  // DROP_DLEAY를 확인하는 타이머 변수
  int timer; 

  // 게임 시작 시간을 기록
  std::chrono::steady_clock::time_point start_time;

  // 게임 시작 후 경과된 시간 정보
  std::string elapse_time_info;

  // Tetromino의 이름을 배열에 저장
  std::string tetromino_name[7] = {
    "I", "O", "L", "O", "S", "T", "Z"
  };

  // 보드판 위에서 떨어지고 있는 tetromino
  Tetromino current_tetromino = Tetromino::I;

  // current_tetromino 이후 떨어질 다음 tetromino의 정보
  Tetromino next_tetromino = Tetromino::I;

  // 현재 홀드하고 있는 tetromino
  Tetromino hold_tetromino = Tetromino::I;

  // current, next, hold tetromino를 출력할 좌표의 초기 X 값
  // initX[0] = current
  // initX[1] = next
  // initX[2] = hold
  int initX[3] = {1, BOARD_WIDTH+3, BOARD_WIDTH + 9};
  
  // current, next, hold tetromino를 출력할 좌표의 초기 Y값
  int initY = 1;

  // 각 Tetromino의 size에 맞추어 가운데로 정렬한 값
  // Tetromino의 크기는 2, 3, 4 밖에 없으므로
  // dx[Tetromino.size() - 2]로 값을 mapping
  // dx[0] -> (BOARD_WIDTH - Tetromino.size()) / 2
  // dx[1] -> (5 - tetromino.size()) / 2
  int dx[2][3] = { {(BOARD_WIDTH - 2)/2, (BOARD_WIDTH-3)/2, (BOARD_WIDTH-4)/2}, {1, 1, 0}}; // board size 변화에도 맞출 수 있게 변경할 필요 있음

  // current_tetromino의 현재 위치 값
  int current_x, current_y;

  // current_tetromino의 shadow y값
  int shadow_y;

  // 현재 hold 중인 테트로미노가 있는지 여부
  bool is_exist_hold_tetromino;

  // 남은 라인 수
  int remain_line_number;

  // Tetromino를 랜덤으로 생성해 반환하는 함수
  Tetromino create_random_tetromino();

  // current_tetro의 shadow shape을 출력할 y값을 계산
  int cal_shadow_y();

  // 떨어지지 않는 테트로미노를 보드판에 갱신, 다음 테트로미노를 가져온다.
  void lock_and_spawn_next();

  // 테트로미노 정보를 갱신하는 함수
  void update_tetromino();

  // 현재 테트로미노와 홀드 테트로미노를 교환하는 함수
  void swap_hold_tetromino();

  // 키보드 입력을 처리해 정보를 갱신하는 함수
  void handle_input();

  // x의 값이 유효한지 확인하는 함수
  bool is_valid_x(int x);
  
  // 현재 지울 라인을 찾아 지우고 보드판을 아래로 내림
  bool check_clear_line();

  // 게임이 종료되어야 하는 조건인지 확인
  bool check_game_over();

  // 경과된 시간 정보를 "분:초:밀리초"의 문자열로 반환하는 함수
  std::string format_time();

  // 현재 보드판 정보 출력
  void draw_board_info();

  // 테트로미노를 화면에 출력
  void draw_tetromino_info();

  // 남은 라인 수, 게임 플레이 시간 등을 출력한다.
  void draw_game_info();

  // 게임 승리 화면을 출력함
  void draw_victory_display();

  // 게임 패배 화면을 출력함
  void draw_defeat_display();

public:
  // 게임의 한 프레임을 처리한다.
  void update();

  // 게임 화면을 그린다.
  void draw();

  // 게임 루프가 종료되어야 하는지 여부를 반환한다.
  bool shouldExit();

  Game();
};
#endif