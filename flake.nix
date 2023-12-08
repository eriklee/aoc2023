{
  description = "aoc2023";
  inputs = {
    nixpkgs = { url = "github:NixOS/nixpkgs/nixpkgs-unstable"; };
    flake-utils = { url = "github:numtide/flake-utils"; };
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs { inherit system; };
      in
      {
        devShell = pkgs.mkShell {
          buildInputs = [
            pkgs.fmt
            pkgs.boost181
            pkgs.cmake
            pkgs.ninja
            pkgs.gdb
            pkgs.lldb
            pkgs.libllvm
            pkgs.bear
            pkgs.clang-tools_16
            pkgs.clang_16
            pkgs.catch2_3
            pkgs.hyperscan
            pkgs.mimalloc
            pkgs.gbenchmark
            pkgs.libhwy
          ];
          shellHook = ''
            '';
          hardeningDisable = [ "fortify" ];
        };
      });
}
