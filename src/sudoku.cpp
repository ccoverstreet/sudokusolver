#include "./sudoku.h"

#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <set>
#include <tuple>
#include <random>

void SudokuGrid::print() {
	std::cout << "-------------------\n";
	for (int i = 0; i < 9; i++) {
		std::cout << "|";
		for (int j = 0; j < 9; j++) {
			std::cout << int(grid[i][j]) << "|";
		}

		std::cout << "\n";
		std::cout << "-------------------\n";
	}
}

void SudokuGrid::print_basic() {
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			std::cout << int(grid[i][j]);
		}
		std::cout << "\n";
	}
}

SudokuGrid SudokuGrid_from_string(const char *str) {
	auto length = strlen(str);
	SudokuGrid grid;
	for (int k = 0; k < length; k++) {
		int i = k / 9;
		int j = k % 9;
		grid.grid[i][j] = str[k] - '0';
	}

	return grid;
}

SudokuGrid SudokuGrid_from_file(const char *filename) {
	std::string content = "";
	std::string line;

	std::ifstream file(filename);

	while(std::getline(file, line)) {
		content += line;
	}

	file.close();


	return SudokuGrid_from_string(content.c_str());
}

bool SudokuGrid_generate_inner(
		SudokuGrid &grid, 
		std::default_random_engine &gen,
		int n_filled,
		int depth) {

	if (depth == n_filled) {
		return true;
	}

	auto cand = generate_candidates(grid);
	size_t best_size = 100;
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (grid.grid[i][j] != 0) continue;

			size_t size = cand.grid[i][j].size();
			if (size == 0) return false;
			if (size < best_size) {
				best_size = size;
			}
		}
	}

	if (best_size == 100) return true;

	std::vector<std::vector<uint8_t>> min_cands;
	std::vector<std::vector<int>> min_cands_coords;
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (cand.grid[i][j].size() > 0) {
				min_cands.push_back(cand.grid[i][j]);
				min_cands_coords.push_back({i, j});
			}
		}
	}

	// Keep trying forever
	// This is probably bugged, but need to check performance
	std::uniform_int_distribution<int> min_indices(0, min_cands.size()-1);
	for (int tries_outer = 0; tries_outer < min_cands.size(); tries_outer++) {
		auto min_i = min_indices(gen);
		auto i = min_cands_coords[min_i][0];
		auto j = min_cands_coords[min_i][1];

		for (int tries_inner = 0; tries_inner < 10; tries_inner++) {
			std::uniform_int_distribution<int> min_cand_ind(0, min_cands[min_i].size()-1);
			auto k = min_cand_ind(gen);
			grid.grid[i][j] = min_cands[min_i][k];

			auto check_copy = grid;
			bool solvable = SudokuGrid_solve_recursive(check_copy);
			if (!solvable) {
				grid.grid[i][j] = 0;
				continue;
			};

			if (SudokuGrid_generate_inner(grid, gen, n_filled, depth+1)) return true;
			grid.grid[i][j] = 0;
		}

		grid.grid[i][j] = 0;
	}

	//grid.grid[best_i][best_j] = 0; // Reset
	return false;
}

SudokuGrid SudokuGrid_generate(int n_filled_spots) {
	std::random_device r;
	std::default_random_engine gen(r());
	std::uniform_int_distribution<int> val_dist(1, 9);
	std::uniform_int_distribution<int> pos_dist(0, 80); // 1D index of grid


	auto p1 = pos_dist(gen);
	auto p1_i = p1 / 9;
	auto p1_j = p1 % 9;

	auto grid = SudokuGrid();

	grid.grid[p1_i][p1_j] = val_dist(gen);

	while (true) {
		auto probe = grid;
		bool success = SudokuGrid_generate_inner(probe, gen, n_filled_spots, 1);
		if (success) return probe;
	}
		
	return grid;
}

void CheckResults::print() {
	if (correct) {
		std::cout << msg << "\n";
	} else {
		std::cout << "Grid is incorrect" << "\n";
		std::cout << msg << "\n";
	}
}



struct SolverState {
	SudokuGrid grid;
	bool correct;
};

bool SudokuGrid_solve_recursive_inner(SudokuGrid &grid) {
	auto cand = generate_candidates(grid);
	int best_i = -1;
	int best_j = -1;
	size_t best_size = 100;

	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (grid.grid[i][j] != 0) continue;

			size_t size = cand.grid[i][j].size();
			if (size == 0) return false;
			if (size < best_size) {
				best_size = size;
				best_i = i;
				best_j = j;
			}
		}
	}

	// No empty cells so good to return
	if (best_i == -1) return true;

	for (uint8_t val : cand.grid[best_i][best_j]) {
		grid.grid[best_i][best_j] = val;
		if (SudokuGrid_solve_recursive_inner(grid)) return true;
	}

	grid.grid[best_i][best_j] = 0; // Reset before next check
	return false;
}

bool SudokuGrid_start_is_valid(SudokuGrid &grid) {
	// Check for repeated digits
	for (int i = 0; i < 9; i++) {
		std::set<uint8_t> vals;
		for (int j = 0; j < 9; j++) {
			auto val = grid.grid[i][j];
			if (val == 0) continue;

			auto count = vals.count(val);

			if (count != 0) {
				return false;
			}

			vals.insert(val);
		}
	}

	for (int j = 0; j < 9; j++) {
		std::set<uint8_t> vals;
		for (int i = 0; i < 9; i++) {
			auto val = grid.grid[i][j];
			if (val == 0) continue;

			auto count = vals.count(val);

			if (count != 0) {
				return false;
			}

			vals.insert(val);
		}
	}

	for (int box_i; box_i < 3; box_i++) {
		for (int box_j; box_j < 3; box_j++) {
			std::set<uint8_t> vals;
			for (int i = box_i*3; i < box_i*3+3; i++) {
				for (int j = box_j*3; j < box_j*3+3; box_j++) {
					auto val = grid.grid[i][j];
					if (val == 0) continue;

					auto count = vals.count(val);

					if (count != 0) {
						return false;
					}
					vals.insert(val);
				}
			}
		}
	}

	return true;
}

bool SudokuGrid_solve_recursive(SudokuGrid &grid) {
	if (!SudokuGrid_start_is_valid(grid)) {
		return false;
	}

	return SudokuGrid_solve_recursive_inner(grid);
}



CheckResults SudokuGrid_check(const SudokuGrid &grid) {
	// Check boxes
	for (int box_i = 0; box_i < 3; box_i++) {
		for (int box_j = 0; box_j < 3; box_j++) {
			std::set<uint8_t> vals;

			for (int i = box_i*3; i < box_i*3 + 3; i++) {
				for (int j = box_j*3; j < box_j*3 + 3; j++) {
					// Count should be 0, otherwise fail
					auto count = vals.count(grid.grid[i][j]);

					if (count != 0) {
						std::ostringstream out;
						out << "Grid failed box check in box index [" 
							<< box_i << ", " << box_j << "]";
						return CheckResults{
							false,
							out.str()
						};
					}

					vals.insert(grid.grid[i][j]);

				}
			}
		}
	}

	// Check columns
	for (int j = 0; j < 9; j++) {
		std::set<uint8_t> vals;
		for (int i = 0; i < 9; i++) {
			auto count = vals.count(grid.grid[i][j]);

			if (count != 0) {
				std::ostringstream out;
				out << "Grid failed column check in column index " 
					<< j;
				return CheckResults{
					false,
					out.str()
				};
			}

			vals.insert(grid.grid[i][j]);
		}
	}

	for (int i = 0; i < 9; i++) {
		std::set<uint8_t> vals;
		for (int j = 0; j < 9; j++) {
			auto count = vals.count(grid.grid[i][j]);

			if (count != 0) {
				std::ostringstream out;
				out << "Grid failed row check in row index " 
					<< i;
				return CheckResults{
					false,
					out.str()
				};
			}

			vals.insert(grid.grid[i][j]);
		}
	}

	return CheckResults{true, "Grid is correct"};
}



void CandidateGrid::print() {
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			auto size = grid[i][j].size();
			for (int k = 0; k < size; k++) {	
				std::cout << int(grid[i][j][k]);
			}

			for (int p_ind = 0; p_ind < 9 - size; p_ind++) {
				std::cout << " ";
			}
			std::cout << "|";
		}


		std::cout << "\n";
		std::cout << "------------------------------------------------------------------------------------------\n";
	}
}

void print_vec(const std::vector<uint8_t> &vec) {
	for (const auto &v : vec) {
		std::cout << int(v);
	}
	std::cout << "\n";
}


std::vector<uint8_t> collapse_cand_vector(
		const std::vector<std::vector<uint8_t>> vecs) {

	std::vector<uint8_t> out;

	for (int i = 1; i <= 9; i++) {
		bool is_in = false;
		for (const auto &v : vecs) {
			is_in = std::find(v.begin(), v.end(), i) != v.end();
			if (is_in) break;
		}

		if (is_in) continue;

		out.push_back(i);
	}


	return out;
}


std::vector<uint8_t> get_candidates_for_spot(
		const SudokuGrid &grid, int spot_i, int spot_j) {

	int box_i = spot_i / 3;
	int box_j = spot_j / 3;

	// Check for taken box values
	std::vector<uint8_t> box_taken = {};
	for (int i = box_i*3; i < box_i*3 + 3; i++) {
		for (int j = box_j*3; j < box_j*3 + 3; j++) {
			if (grid.grid[i][j] == 0) continue;
			box_taken.push_back(grid.grid[i][j]);
		}
	}

	std::vector<uint8_t> column_taken = {};
	for (int i = 0; i < 9; i++) {
		if (grid.grid[i][spot_j] == 0) continue;

		column_taken.push_back(grid.grid[i][spot_j]);
	}

	std::vector<uint8_t> row_taken = {};
	for (int j = 0; j < 9; j++) {
		if (grid.grid[spot_i][j] == 0) continue;

		row_taken.push_back(grid.grid[spot_i][j]);
	}


	return collapse_cand_vector({box_taken, row_taken, column_taken});
}

CandidateGrid generate_candidates(const SudokuGrid &grid) {
	CandidateGrid cand;

	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (grid.grid[i][j] != 0) continue;

			cand.grid[i][j] = get_candidates_for_spot(grid, i, j);
		}
	}

	return cand;
}

