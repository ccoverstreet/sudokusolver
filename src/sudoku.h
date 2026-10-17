#pragma once

#include <cstdint>
#include <vector>
#include <string>

void print_vec(const std::vector<uint8_t> &vec);

struct SudokuGrid {
	uint8_t grid[9][9];

	void print();
};

SudokuGrid SudokuGrid_from_string(const char* str);
SudokuGrid SudokuGrid_from_file(const char* filename);

struct CheckResults {
	bool correct;
	std::string msg;

	void print();
};

CheckResults SudokuGrid_check(const SudokuGrid &grid);
SudokuGrid SudokuGrid_solve(SudokuGrid grid);
bool SudokuGrid_solve_recursive(SudokuGrid &grid);

struct CandidateGrid {
	std::vector<uint8_t> grid[9][9];

	void print();
};

// This candidates are only doing the basic check for boxs, rows, and columns
CandidateGrid generate_candidates(const SudokuGrid &grid);
std::vector<uint8_t> get_candidates_for_spot(
		const SudokuGrid &grid, int spot_i, int spot_j);
std::vector<uint8_t> collapse_cand_vector(
		const std::vector<std::vector<uint8_t>> vecs);

// This candidate applies more advanced techniques
// TODO: brute force backtracking version
CandidateGrid trim_candidates_advanced(CandidateGrid cand);

