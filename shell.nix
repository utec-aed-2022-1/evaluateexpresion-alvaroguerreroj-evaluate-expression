{ pkgs ? import <nixpkgs> { } }:

with pkgs;

mkShell {
  buildInputs = [
  ];

  nativeBuildInputs = [
    clang_14
    gcc

    cling
    clang-tools
    valgrind
    gdb
    cgdb

    just
    meson
    ninja
  ];
}
