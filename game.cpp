#include "game.h"
#include "console.h"
#include <cstdlib>
using namespace std;


// 생성자
Game::Game(){
  // 게임 보드 정보 초기화
  for(int i = 0; i < BOARD_WIDTH; i++)
  for(int j = 0; j < BOARD_HEIGHT; j++)
  board_[i][j] = 0;

  // 게임을 시작한 시간을 기록
  start_time = chrono::steady_clock::now();

  // 현재 테트로미노를 생성
  current_tetromino = create_random_tetromino();
  current_x = initX[0] + dx[0][current_tetromino.size() - 2];
  current_y = initY;

  // shadow shape을 출력할 y좌표를 계산한다.
  shadow_y = cal_shadow_y();

  // 다음 테트로미노를 생성
  next_tetromino = create_random_tetromino();

  // 현재 hold 중인 테트로미노가 없음
  is_exist_hold_tetromino = 0;

  // 현재 delay된 횟수
  timer = 0;

  // 지워야될 라인의 수 40으로 초기화
  remain_line_number = LINES;
}

// 현재 보드판의 정보를 콘솔에 출력하는 함수 
void Game::draw(){
  // 보드판 정보를 출력
  draw_board_info();

  // 각 테트로미노의 정보 출력
  draw_tetromino_info();

  // 남은 라인 수 또는 게임 경과 시간 등을 출력
  draw_game_info();

  // 게임 종료 조건이 일치할 경우 각각 승리, 패배 조건에 맞는 화면을 출력
  if(remain_line_number == 0) draw_victory_display();
  if(current_y != shadow_y && check_game_over()) draw_defeat_display();
}

// 게임 정보를 갱신해주는 함수
void Game::update(){
  handle_input();
  if(current_y == shadow_y) lock_and_spawn_next();
  
  // 지울 수 있는 라인을 모두 지운다.
  if(check_clear_line()) remain_line_number--;

  // delay가 DROP_DELAY만큼 되지 않았을 경우 return
  if(timer++ < DROP_DELAY) return;
  
  // 타이머를 다시 0으로 초기화
  timer = 0;
  // 현재 테트로미노의 위치를 아래로 1칸 갱신
  current_y++;
}

// 게임 루프가 종료되어야 하는지 여부를 반환하는 함수
bool Game::shouldExit(){
  // 게임 종료 조건과 일치할 경우 true 반환
  if(remain_line_number == 0 || (current_y != shadow_y && check_game_over())) return 1;
  return 0;
}

// 테트로미노의 이름 배열을 랜덤 인덱스로 접근해
// 테트로미노를 생성한 뒤, original() 함수를 호출하여 저장한 뒤,
// 그 이름의 원래 크기와 모양이 저장되어있는 테트로미노를 반환한다. 
Tetromino Game::create_random_tetromino(){
  Tetromino *create_tetromino = Tetromino(tetromino_name[rand()%7], 0, "O").original();
  return *create_tetromino;
}

// 현재 게임 플레이 경과 시간을 반환한다.
string Game::format_time(){
  // 현재 시간 정보를 저장
  chrono::steady_clock::time_point current_time = chrono::steady_clock::now();
  // 게임 시작 시간 차를 millisecond로 저장한다.
  chrono::milliseconds elapse_time = chrono::duration_cast<chrono::milliseconds>(current_time-start_time);
  
  // 분
  chrono::minutes minutes = chrono::duration_cast<chrono::minutes>(elapse_time);
  elapse_time -= minutes;

  // 초
  chrono::seconds seconds = chrono::duration_cast<chrono::seconds>(elapse_time);
  elapse_time -= seconds;

  // 시간 정보를 00:00:00으로 format
  auto minutes_count = minutes.count();
  auto seconds_count = seconds.count();
  auto millisec_count = elapse_time.count();
  string duration{};
  if(minutes_count/10 == 0) duration.push_back('0');
  duration += to_string(minutes_count)+":";
  if(seconds_count/10 == 0) duration.push_back('0');
  duration += to_string(seconds_count)+":";
  if(millisec_count/10 == 0) duration += "00";
  duration += to_string(millisec_count/10);
  
  return duration;
}

// 현재 테트로미노를 더 놓을 수 없거나, 이동시킬 수 없는지 검사하고
// 게임이 종료되어야하는지 여부를 반환한다.
bool Game::check_game_over(){
  // 테트로미노가 있는 곳인데 이미 블럭이 쌓여있는 경우
  for(int i = 0; i < current_tetromino.size(); i++)
  for(int j = 0; j < current_tetromino.size(); j++)
  if(!current_tetromino.check(i,j)) continue;
  else if(board_[current_x + i - 1][current_y + j - 1]) return 1; // game over
  
  return 0;
}

// 현재 보드판의 경계와 정보를 출력하는 함수
void Game::draw_board_info(){
  // 게임 보드판을 출력
  console::drawBox(0, 0, BOARD_WIDTH+1, BOARD_HEIGHT+1);
  // next_tetromino 정보를 출력할 box를 출력
  console::drawBox(initX[1]-1, 0, initX[1]+4, 5);
  // next box의 이름 출력
  console::draw(initX[1], 0, "next");
  // hold_tetromino 정보를 출력할 box를 출력
  console::drawBox(initX[2]-1, 0, initX[2]+4, 5);
  // hold box의 이름 출력
  console::draw(initX[2], 0, "hold");

  // 보드판에 현재 쌓여있는 테트로미노 블럭들을 출력한다.
  for(int i = 0; i < BOARD_WIDTH; i++)
  for(int j = 0; j < BOARD_HEIGHT; j++)
  if(board_[i][j])
  console::draw(i+1, j+1, BLOCK_STRING);
}

// 테트로미노들의 정보를 화면에 출력
void Game::draw_tetromino_info(){
  current_tetromino.drawAt(SHADOW_STRING, current_x, shadow_y);
  current_tetromino.drawAt(BLOCK_STRING, current_x, current_y);
  next_tetromino.drawAt(BLOCK_STRING, initX[1] + dx[1][next_tetromino.size()-2], initY + dx[1][next_tetromino.size()-2]);
  // 현재 홀드한 테트로미노가 있다면 출력
  if(is_exist_hold_tetromino) 
    hold_tetromino.drawAt(BLOCK_STRING, initX[2]+dx[1][hold_tetromino.size()-2], initY + dx[1][hold_tetromino.size()-2]);
}

// 남은 라인 수, 게임 경과 시간 등 정보를 출력
void Game::draw_game_info(){
  string remain_line_info = to_string(remain_line_number) + " lines left";
  elapse_time_info = format_time();
  // remain_line_info가 정렬된 위치가 음수일 경우 0으로 바꾼다.
  int line_info_x = (BOARD_WIDTH - remain_line_info.size()) / 2;
  console::draw(max(line_info_x, 0), BOARD_HEIGHT+2, remain_line_info);
  console::draw((BOARD_WIDTH - elapse_time_info.size()) / 2 + 1, BOARD_HEIGHT+3, elapse_time_info);
}

// 게임 승리 시 문구를 출력
void Game::draw_victory_display(){
  string victory_string = "You Win";
  console::draw((BOARD_WIDTH-victory_string.size())/2+1, BOARD_HEIGHT/2, victory_string);
  console::draw((BOARD_WIDTH-elapse_time_info.size())/2+1, BOARD_HEIGHT/2+1, elapse_time_info);
}

// 게임 패배 시 문구를 출력
void Game::draw_defeat_display(){
  string defeat_string = "You Lost";
  console::draw((BOARD_WIDTH-defeat_string.size())/2+1, BOARD_HEIGHT/2, defeat_string);
}

// 현재 테트로미노의 shadow shape의 y값을 계산해 반환
int Game::cal_shadow_y(){
  // 현재 테트로미노가 더 이상 떨어질 수 없는 위치의 y좌표
  int shadow_height;
  // 현재 위치부터 아래로 내려가면서 반복
  for(shadow_height = current_y; shadow_height <= BOARD_HEIGHT; shadow_height++)
  for(int i = 0; i < current_tetromino.size(); i++)
  for(int j = 0; j < current_tetromino.size(); j++){
    if(!current_tetromino.check(i, j)) continue;
    // 테트로미노가 있어야할 위치에 이미 블럭이 있거나
    // 보드판의 높이를 벗어날 경우, 그 이전 위치를 반환한다.
    // 잘못된 메모리 접근 방지를 위해 보드판 높이 검사를 먼저 한다.
    if(j + shadow_height > BOARD_HEIGHT || board_[i + current_x - 1][j + shadow_height - 1])
      return shadow_height - 1;
  }  
  // 이 값이 반환되면 현재 놓을 수 없다는 뜻이 됨
  return -1;
}

// 더 떨어지지 못하는 current_tetromino의 위치 정보를 보드에 갱신해주고
// 테트로미노의 정보를 갱신한다.
void Game::lock_and_spawn_next(){
  // 보드판을 갱신
  for(int i = 0; i < current_tetromino.size(); i++)
  for(int j = 0; j < current_tetromino.size(); j++)
  if(current_tetromino.check(i, j))
  board_[current_x + i - 1][current_y + j - 1] = 1;

  // 다음 테트로미노를 가져옴
  update_tetromino();
}

// next_tetromino의 정보를 current_tetromino로 가져오고
// next_tetromino는 새로운 테트로미노를 생성한다.
void Game::update_tetromino(){
  // next_tetromino를 current_tetromino로 가져옴
  current_tetromino = next_tetromino;
  current_x = initX[0] + dx[0][current_tetromino.size()-2];
  current_y = initY;

  // shadow_y를 갱신
  shadow_y = cal_shadow_y();

  // 새로운 테트로미노 생성
  next_tetromino = create_random_tetromino();
}

// 홀드하고 있는 테트로미노와 현재 테트로미노를 교환
void Game::swap_hold_tetromino(){
  // 이미 홀드한 테트로미노가 있을 때,
  if(is_exist_hold_tetromino){
    swap(current_tetromino, hold_tetromino);
    hold_tetromino = *(hold_tetromino.original());
  }
  // 홀드한 테트로미노가 없을 때
  else
  {
    is_exist_hold_tetromino = 1;
    hold_tetromino = *(current_tetromino.original());
    update_tetromino();
  }
  while(!is_valid_x(current_x)) current_x < BOARD_WIDTH / 2 ? current_x++ : current_y--;
}

// x값이 유효한지 확인하는 함수
bool Game::is_valid_x(int x){
  for(int i = 0; i < current_tetromino.size(); i++)
  for(int j = 0; j < current_tetromino.size(); j++){
    // 테트로미노가 없는 곳이면 넘어감
    if(!current_tetromino.check(i, j)) continue;
    if(x+i <= 0 || x+i > BOARD_WIDTH || board_[x+i-1][current_y + j - 1]) return 0;
  }
  return 1;
}

// 지울 수 있는 라인을 찾아 지우고 성공 여부를 반환한다.
bool Game::check_clear_line(){
  // 지워야할 라인의 높이
  int clear_line_height = -1; 

  // 현재 지울 라인의 높이를 찾는다.
  for(int i = 0; i < BOARD_HEIGHT; i++){
    bool flag = 1;
    for(int j = 0; j < BOARD_WIDTH; j++) if(!board_[j][i]) flag = 0;
    if(flag){ clear_line_height = i; break;}
  }

  // 지울 라인을 찾지 못했다면 false 반환
  if(clear_line_height == -1) return 0;

  // 현재 지울 라인의 위 라인들을 하나 씩 아래로 내림
  for(int i = clear_line_height; i > 0; i--)
  for(int j = 0; j < BOARD_WIDTH; j++)
  board_[j][i] = board_[j][i-1];

  // 라인을 지웠음, true 반환
  return 1;
}

// 키보드 입력을 처리해 정보를 갱신하는 함수
void Game::handle_input(){
  if(console::key(console::K_RIGHT))
  {
    // 오른쪽으로 이동이 가능한지 판별 후 오른쪽으로 이동 또는 현 위치 유지
    current_x = is_valid_x(current_x+1) ? current_x+1 : current_x;
  }
  else if(console::key(console::K_LEFT))
  {
    // 왼쪽으로 이동이 가능한지 판별 후 왼쪽으로 이동 또는 현 위치 유지
    current_x = is_valid_x(current_x-1) ? current_x-1 : current_x;
  }
  else if(console::key(console::K_UP))
  {
    // current_tetromino의 위치를 도착해야하는 위치인 shadow_y로 갱신
    current_y = shadow_y;
  }
  else if(console::key(console::K_DOWN))
  {
    // current_tetromino의 위치를 아래로 1칸 갱신
    current_y++;
  }
  else if(console::key(console::K_X))
  {
    // 테트로미노가 시계방향으로 회전했을 때,
    // 유효한 위치가 아닌 경우 보드판을 벗어나므로 유효한 위치로 갱신
    current_tetromino = current_tetromino.rotatedCW();
    while(!is_valid_x(current_x)) current_x < BOARD_WIDTH / 2 ? current_x++ : current_x--;
  }
  else if(console::key(console::K_Z))
  {
    // 테트로미노가 반시계방향으로 회전했을 때,
    // 유효한 위치가 아닌 경우 보드판을 벗어나므로 유효한 위치로 갱신
    current_tetromino = current_tetromino.rotatedCCW();
    while(!is_valid_x(current_x)) current_x < BOARD_WIDTH / 2 ? current_x++ : current_x--; 
  }
  else if(console::key(console::K_SPACE))
  {
    // hold_tetromino와 current_tetromino를 swap
    swap_hold_tetromino();
  }
  else if(console::key(console::K_ESC))
  {
    // 게임을 즉시 종료
    exit(1);
  }
  // current_tetromino의 shadow shape의 y값을 갱신
  shadow_y = cal_shadow_y();
}