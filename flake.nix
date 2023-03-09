{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }: flake-utils.lib.eachSystem [ "aarch64-darwin" ] (system:
    let
      pkgs = nixpkgs.legacyPackages.${system};
      sqlite = pkgs.sqlite.override { interactive = true; };
    in
    {
      packages.default = pkgs.stdenv.mkDerivation {
        name = "sqlite-levenshtein";
        src = ./.;
        installPhase = ''
          mkdir -p "$out"/lib
          cp levenshtein.dylib "$out"/lib
        '';
      };

      devShells.default = pkgs.mkShell {
        buildInputs = [
          pkgs.stdenv.cc
          sqlite
        ];
      };
    });
}
