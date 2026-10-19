#include "./sudoku.h"

#include <iostream>
#include <cstring>

bool is_numeric_string(const char *str) {
	for (int i = 0; i < strlen(str); i++) {
		auto is_digit = std::isdigit(str[i]);
		if (!is_digit) return false;
	}

	return true;
}

int main(int argc, char *argv[]) {
	if (argc == 1) {
		std::cout << "Sudoku Generator\n";
		std::cout << "Arguments:\n";
		std::cout << "\t-h,--help  \tPrints this message\n";
		std::cout << "\t# of filled\tMust be less than or equal to 81\n";
		std::cout << "\t           \tMinumum of 17 for unique solution\n";

		return 0;
	}
	
	const char *arg_n = argv[1];
	bool is_numeric = is_numeric_string(arg_n);
	if (!is_numeric) {
		std::cout << "Argument must be an integer in range [0, 81] (>=17 for unique solution)\n";
		return 1;
	}

	int n_filled = std::stoi(arg_n);

	auto grid = SudokuGrid_generate(n_filled);

	//std::cout << "Generated grid:\n";

	//grid.print();

	auto solve_grid = grid;
	auto solved = SudokuGrid_solve_recursive(solve_grid);

	//solve_grid.print();
	grid.print_basic();
	//solve_grid.print_basic();


	return 0;
}
