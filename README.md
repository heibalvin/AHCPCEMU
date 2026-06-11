# AHCPCEMU
Amstrad CPC Emulator written in C++ and SDL3

## Install, Build, Run

Install
```
# 1. Install CMake (if you don't have it)
brew install cmake

# 2. Install SDL3 (Development Library)
brew install sdl3

# 3. Insall git
brew install git
```

Build
```
# Clone the code repo
git clone https://github.com/heibalvin/AHCPCEMU.git

# 1. Create a clean Build target folder and step into it
cd AHCPCEMU

# 2. Run the CMake generation pass
# This locates SDL3 via Homebrew automatically and sets up macOS bundle architectures
cmake .

# 3. Compile the code into the final executable package
cmake --build .
```

Run
```
open AHCPCPEMU.app
```

Clean the build folder
```
cmake --build Build --target clean
```

# Create the Resources directory if it doesn't exist
mkdir -p Resources

# Create empty placeholder files so the bundle step doesn't fail
touch Resources/OS_6128.ROM
touch Resources/BASIC_6128.ROM
```

## Projet Structure

```
AHCPCPEMU/
├── CMakeLists.txt
├── Resources/
├── Build/
├── Include/
│   ├── cpcapp.h
└── Source/
    ├── main.cpp
    └── cpcapp.cpp
```

