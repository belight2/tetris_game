#include "console.h"
#include "game.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
using namespace std;
const char nl = '\n';

int main() {
 srand((unsigned int)time(nullptr));
  console::init();
  Game game;
  string s;
  while (!game.shouldExit()) {
    console::clear();

    game.update();
    game.draw();
    // wait은 여기서만 호출되어야 한다.
    console::wait();
  } 
}