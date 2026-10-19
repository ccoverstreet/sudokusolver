all: sudokusolver sudokugen

sudokusolver: src/*
	$(CXX) -O3 -o sudokusolver src/main.cpp src/sudoku.cpp

sudokugen: src/*
	$(CXX) -O3 -o sudokugen src/sudokugen.cpp src/sudoku.cpp

run: sudokusolver
	./sudokusolver

