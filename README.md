# HostCTL
A simple GUI tool to automatically configure local development domains for XAMPP/WAMP projects.

> Note: This was a school assignment, I took the the base of my [dll injector project](https://github.com/msh31/imgui-dll-injector) to get started with the GUI functionality and left out anything not required.
---

<img src="https://i.imgur.com/GXa23eL.png" alt="preview image">

## Information
1. Make sure you change your username/group in MAMP's ``httpd.conf`` file.
2. Make sure to run as Administator or launch with sudo on Mac/Linux
3. Unless you changed the port in MAMP's config, the default port is ``8888`` and is required to be included in the url, e.x: ``test.local:8888``

## Building
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
```
