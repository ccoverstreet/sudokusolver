#include <iostream>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <string>
#include <cstring>
#include <fstream>

#include "./sudoku.h"


int main(int argc, char *argv[]) {
	if (argc == 1) {
		std::cout << "Provide filenames of sudoku puzzles in a text format as arguments\n";
		return 0;
	}

	for (int i = 1; i < argc; i++) {
		std::cout << "Reading puzzle from " << argv[i] << "\n";
		auto start = SudokuGrid_from_file(argv[i]);
		auto solved_grid = start;

		std::cout << "Solving puzzle from " << argv[i] << "\n";
		bool solved = SudokuGrid_solve_recursive(solved_grid);


		std::cout << "Starting grid:\n";
		start.print();

		std::cout << "\n";

		if (!solved) {
			std::cout << "Unable to solve sudoku grid\n";
		} else {
			std::cout << "Solved grid\n";
		}

		std::cout << "\nEnding grid:\n";
		solved_grid.print();

		std::cout << "\n";
		//auto results = SudokuGrid_check(end);
		//results.print();
	}

	return 0;
}
