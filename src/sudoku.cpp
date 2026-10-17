#include "./sudoku.h"

#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <set>
#include <tuple>

void SudokuGrid::print() {
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

void CheckResults::print() {
	if (correct) {
		std::cout << msg << "\n";
	} else {
		std::cout << "Grid is incorrect" << "\n";
		std::cout << msg << "\n";
	}
}

/*
SudokuGrid SudokuGrid_solve(SudokuGrid grid) {
	std::vector<SolverState> backups;

	int counter = 0;
	while (true) {
		auto cand = generate_candidates(grid);
		int loop_min_size = 100; // Random value as placeholder
		int loop_min_i, loop_min_j = 0;

		for (int i = 0; i < 9; i++) {
			for (int j = 0; j < 9; j++) {
				if (grid.grid[i][j] != 0) continue;
				auto size = cand.grid[i][j].size();
				if (size < loop_min_size) {
					std::cout << int(grid.grid[i][j]) << "\n";
					std::cout << "New min" << size << " " << i << j << "\n";
					loop_min_size = size;
					loop_min_i = i;
					loop_min_j = j;
				}
				if (cand.grid[i][j].size() == 1) {
					grid.grid[i][j] = cand.grid[i][j][0];

					goto loop_end;
				}
			}
		}
//loop_end:;

		if (loop_min_size == 1) {
			// Just pick the lowest value for now.
			// Will liekly need to track multiple
			// for brute force searching in the future
			grid.grid[loop_min_i][loop_min_j] = cand.grid[loop_min_i][loop_min_j][0];
		} else if (loop_min_size > 1 && loop_min_size != 100) {
			// Need to save current state for backtracking

		} else (loop_min_size == 0 && grid.grid[loop_min_i][loop_min_j] > 0) {
			// We have run into an invalid path, need to backtrack

		}

		if (loop_min_size == 100) {
			// No more possible moves	
			break;
		}

		std::cout << "\n";
		cand.print();
		
		std::cout << "\n";
		grid.print();

		counter++;
		if (counter > 4) break;
	}

	return grid;
}
*/



std::vector<std::tuple<int, int>> get_indices_of_min_candidates(CandidateGrid cand,
		SudokuGrid grid) {
	bool invalid_solution_flag = false;

	// Identify minimum size
	int min_size = 100; // arbitrary value
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (grid.grid[i][j] > 0) continue;

			auto size = cand.grid[i][j].size();
			if (size == 0 && grid.grid[i][j] == 0) {
				// invalid solution
				return {};
			}

			if (size < min_size) {
				min_size = size;
			}
		}
	}

	//cand.print();
	std::vector<std::tuple<int, int>> out;

	// Collect indices of all min_candidates
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			auto size = cand.grid[i][j].size();
			if (size == min_size) {
				out.push_back({i, j});
			}
		}
	}

	return out;
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

