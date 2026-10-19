{
  description = "Sudoku Solver CLI";

  inputs = {
    nixpkgs.url = "https://channels.nixos.org/nixpkgs-unstable/nixexprs.tar.zst";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
  flake-utils.lib.eachDefaultSystem (system:
    let 
      pkgs = nixpkgs.legacyPackages.${system};
      solverProgram = pkgs.stdenv.mkDerivation {
        pname = "sudokusolver";
        version = "0.1.0";
        src = self;
        nativeBuildInputs = [ pkgs.gnumake ];
        buildPhase = ''
          make sudokusolver
          '';
        installPhase = ''
          mkdir -p $out/bin
          cp sudokusolver $out/bin/
          '';
      };
      genProgram = pkgs.stdenv.mkDerivation  {
        pname = "sudokugen";
        version = "0.1.0";
        src = self;
        nativeBuildInputs = [ pkgs.gnumake ];
        buildPhase = ''
          make sudokugen
          '';
        installPhase = ''
          mkdir -p $out/bin
          cp sudokugen $out/bin/
          '';

      };
    in
    {

      packages = {
        solver = solverProgram;
        gen = genProgram;

        all = pkgs.symlinkJoin {
          name = "sudokutools";
          paths = [ solverProgram genProgram ];
        };

        default = self.packages.${system}.all;
      };

      apps = {
        solver = {
          type = "app";
          program = "${solverProgram}/bin/sudokusolver";
        };
        gen = {
          type = "app";
          program = "${genProgram}/bin/sudokugen";
        };
        default = self.apps.${system}.solver;
      };

    });
}
