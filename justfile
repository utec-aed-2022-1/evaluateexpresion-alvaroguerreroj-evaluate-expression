# -*- mode: just; -*-

build_dir := "build/"

build: _build_exists
    meson compile -C {{build_dir}}

setup *args:
    meson setup {{args}} {{build_dir}}

resetup *args: clean
    just setup {{args}}

clean:
    rm -rf {{build_dir}}

format: _build_exists
    ninja -C {{build_dir}} clang-format

_build_exists:
    test -d {{build_dir}}
