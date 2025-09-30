# HostCTL
A simple GUI tool to automatically configure local development domains for XAMPP/WAMP projects.

> Note: This was a school assignment, I took the the base of my [dll injector project](https://github.com/msh31/imgui-dll-injector) to get started with the GUI functionality and left out anything not required.
---

## Info
##### All source code for the main application is in ``main.cpp``, not at all OOP, all hail procedural programming!

## Getting Started
### Requirements
- C++ 17+ compatible compiler (MSVC, GCC, or Clang)
- CMake 3.20 or higher

### Build & Run
```zsh
git clone --recurse-submodules -j8 https://github.com/msh31/hostctl.git
cd hostctl

cmake -S . -B build
cmake --build build
# optional, built as release:
cmake --build build --config Release