FLAGS = -Werror -std=c++11

main.exe: game.o game.h console.o console.h tetromino.o tetromino.h main.o
	g++ -o main.exe main.o console.o tetromino.o game.o

main.o : main.cpp
	g++ ${FLAGS} -c main.cpp
console.o : console.cpp console.h
	g++ ${FLAGS} -c console.cpp
game.o : game.cpp game.h
	g++ ${FLAGS} -c game.cpp
tetromino.o : tetromino.cpp tetromino.h
	g++ ${FLAGS} -c tetromino.cpp
clean :
	del *.o
	del main.exe
