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
      generatedProgram = pkgs.stdenv.mkDerivation {
        pname = "sudokusolver";
        version = "0.1.0";
        src = self;
        nativeBuildInputs = [ pkgs.gnumake ];
        buildPhase = ''
          make
          '';
        installPhase = ''
          mkdir -p $out/bin
          cp sudokusolver $out/bin/
          '';
      };
    in
    {
      packages.default = generatedProgram;

      apps.default = {
        type = "app";
        program = "${self.packages.${system}.default}/bin/sudokusolver";
      };

    });
}
