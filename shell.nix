{ pkgs ? import <nixpkgs> { } }:

with pkgs;

mkShell {
  buildInputs = [ catch2 microsoft_gsl ];

  nativeBuildInputs = [
    clang_14
    gcc

    cling
    clang-tools
    valgrind
    gdb
    cgdb

    cmake
    just
    meson
    ninja
    pkg-config
  ];
}
