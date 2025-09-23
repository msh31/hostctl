# ImGui Loader
A GUI version of the [dll injector](https://github.com/msh31/dll-injector) project, made with ImGui & GLFW. 
> Note: this started as a personal interest, but for a school assignment I had to make a desktop application with C++ (The DLL injector part was my idea.)


---

## ⚒️ How to build
```zsh
git clone --recurse-submodules https://github.com/msh31/imgui-loader.git
cd imgui-loader

cmake -B build -S .
cmake --build build 
# or to specify release mode
cmake --build build --config Release
```