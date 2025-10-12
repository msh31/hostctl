#!/bin/zsh
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -S . -B build
cmake --build build
ln -sf build/compile_commands.json compile_commands.json
