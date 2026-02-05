# Colony Simulator

A simulator of colonizing the world

## How to run

- Download the latest release
- Unzip it
- Run ColonySimulator or ColonySimulator.exe depending on your system

## How to build

### Without modding support

1. If on Linux, install raylib's dependencies following [this tutorial](https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux#dependencies)
2. You'll need a C/C++ compiler, Git and CMake
3. Run 
```bash
git clone https://github.com/SemkiShow/ColonySimulator
cd ColonySimulator
git submodule update --init --recursive --depth 1 --jobs 8
chmod +x run.sh
./run.sh
```

### With modding support

> [!WARNING]
> Building with modding support requires more dependencies and increases the compile times 2-3 times  
> Use this option only if you really need to

1. If on Linux, install raylib's dependencies following [this tutorial](https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux#dependencies)
2. You'll need a C/C++ compiler, Git and CMake
3. For generating Lua bindings a few additional dependencies are required: clang, libclang, Python and a library clang in Python
   1. Debian
   ```bash
   sudo apt install clang python3 python3-clang
   ```
   2. Fedora
   ```bash
   sudo dnf install clang python3 python3-clang
   ```
4. Run 
```bash
git clone https://github.com/SemkiShow/ColonySimulator
cd ColonySimulator
git submodule update --init --recursive --depth 1 --jobs 8
python3 tools/lua_bindings.py
chmod +x run.sh
./run.sh -M
```

## Modding

> [!WARNING]
> Mods you download may contain viruses. The author is not responsible for any damage caused by using third-party mods

Pretty much all functions accessible in C++ can be accessed in Lua.  
To create a mod, put mod-name.lua or mod-name/main.lua inside of the mods/ folder (it may need to be created first).  

To get a global variable's value defined in C++ use `variable()`. To set the variable to a new value use `variable = value`  

The release contains tools/colsimapi.lua, which is a meta file for the lua lsp  
It can also serve as documentation, though pretty unreadable because of the size (5k+ lines)

Example mod:
```lua
-- Print a mod load message to the console
print("Hello from Lua!")

-- Change the window title
SetWindowTitle("Lua mod")

-- Change the human texture to the ship texture
UnloadTexture(humanTexture())
humanTexture = LoadTexture("resources/textures/ship.png")

-- The Update function called in the game's DrawFrame fuction
-- The name must match exactly for this to work
-- deltaTime must be present, can be under a different name
function Update(deltaTime)
    -- Draw a red rectangle if the game menu is visible
    if gameMenu():IsVisible() then
        DrawRectangleRec(Rectangle(0, 0, 100, 100), Color(255, 0, 0, 255))
    end
end
```
