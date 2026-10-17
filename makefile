sudokusolver: src/*
	g++ -O3 -o sudokusolver src/main.cpp src/sudoku.cpp

run: sudokusolver
	./sudokusolver

