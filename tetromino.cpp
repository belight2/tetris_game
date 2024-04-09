#include "tetromino.h"
#include "console.h"
#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;

Tetromino Tetromino::I = Tetromino("I", 4, "XXXXOOOOXXXXXXXX");
Tetromino Tetromino::T = Tetromino("T", 3, "XOXOOOXXX");
Tetromino Tetromino::O = Tetromino("O", 2, "OOOO");
Tetromino Tetromino::J = Tetromino("J", 3, "OXXOOOXXX");
Tetromino Tetromino::L = Tetromino("L", 3, "XXOOOOXXX");
Tetromino Tetromino::S = Tetromino("S", 3, "XOOOOXXXX");
Tetromino Tetromino::Z = Tetromino("Z", 3, "OOXXOOXXX");

Tetromino::Tetromino(string name, int size, string shape){
  name_ = name;
  size_ = size;
  for(int i = 0; i < size; i++) for(int j = 0; j < size; j++)
    shape_[i][j] = (shape[i*size+j] == 'O'); 
  if(name == "I") original_ = &Tetromino::I;
  else if(name == "T") original_ = &Tetromino::T;
  else if(name == "O") original_ = &Tetromino::O;
  else if(name == "J") original_ = &Tetromino::J;
  else if(name == "L") original_ = &Tetromino::L;
  else if(name == "S") original_ = &Tetromino::S;
  else if(name == "Z") original_ = &Tetromino::Z;
}

Tetromino Tetromino::rotatedCW(){
  string newshape{};
  for(int i = 0; i < size_; i++){
    for(int j = size_-1; j >= 0; j--){
      shape_[j][i] ? newshape.push_back('O') : newshape.push_back('X');
    }
  }
  return Tetromino(name_, size_, newshape);
}

Tetromino Tetromino::rotatedCCW(){
  return rotatedCW().rotatedCW().rotatedCW();
}

void Tetromino::drawAt(string s, int x, int y){
  for(int i = 0; i < size_; i++){
    for(int j = 0; j < size_; j++){
      if(check(i,j)) console::draw(x+i, y+j, s);
    }
  }
}
