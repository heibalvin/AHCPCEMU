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

Build & Run Debug Version
```
# Clone the code repo
git clone https://github.com/heibalvin/AHCPCEMU.git
cd AHCPCEMU

# 1. Regenerate your build files with Dev Mode active
cmake -S . -B Build -DAHC_DEV_MODE=ON

# 2. Compile the binary target
cmake --build Build

# 3. Launch the binary directly through the shell (DO NOT use 'open')
./Build/AHCPCPEMU
```

Build & Run MacOS app Version
```
# 1. Force CMake to switch to full Apple App Bundle packaging rules
cmake -S . -B Build -DAHC_DEV_MODE=OFF

# 2. Recompile
cmake --build Build

# 3. Launch your clean package
open Build/AHCPCPEMU.app
```

Clean the build folder
```
cmake --build Build --target clean
or
rm -rf Build/* 
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

