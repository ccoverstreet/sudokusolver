# Sudoku Solver and Generator

A simple sudoku solver that takes text files as input and a generator that takes a single integer as an input and outputs the grid in the format shown below (same format used for solver).

Example file contents:
```
000067102
097800040
240003005
605970200
800054030
070310068
300081057
018500300
402006900
```

## Usage and Installation

Can be built by running `make` or by using the nix flake (`nix build`)

### Nix

```
nix profile add github:ccoverstreet/sudokusolver
sudokugen 30 > mypuzzle.txt
sudokusolver <filenames>
```


