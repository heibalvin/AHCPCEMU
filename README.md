# AHCPCPEMU — Amstrad CPC Architecture Framework

A modern, multi-platform Amstrad CPC hardware emulator backend core and GUI shell wrapper designed using C++20 and SDL3.

This repository features a decoupled architecture split into a shared core system library (`CPCEMUCORE`), an interactive graphical application (`AHCPCPEMUAPP`), and an automated headless verification test execution runner (`AHCPCPEMU`).

---

## 📦 Directory Structure

```text
AHCPCPEMU/
├── CMakeLists.txt        # Master multi-platform build specification script
├── README.md             # This document
├── .gitignore            # Version control tracking constraints
├── Resources/            # Embedded hardware system binaries
│   ├── OS_6128.ROM
│   └── BASIC_6128.ROM
├── Include/              # Object interface definition headers (.h)
│   ├── cpcapp.h          # Host OS graphical layer coordinator
│   └── cpcemu.h          # Virtual motherboard/bus hardware core
└── Source/               # Implementation files (.cpp)
    ├── main.cpp          # Headless diagnostic app entry point
    ├── main_app.cpp      # Graphical windowed app entry point
    ├── cpcapp.cpp        # Window/Event lifecycle routines
    └── cpcemu.cpp        # Hardware cycle execution matrix

```

---

## 🛠️ Installation & Setup (macOS Prerequisites)

To compile the codebase on macOS, verify your local system contains standard development pipelines and the **SDL3** framework via the Homebrew package utility:

```bash
# Install CMake build tree generation utilities
brew install cmake

# Install the standard SDL3 development libraries
brew install sdl3

```

---

## 🚀 Building & Running the System

Every compilation sequence should be executed safely from the **project root folder** using CMake's modern out-of-source parameters (`-B Build`). This completely isolates your runtime code and prevents clutter from spilling into your source environment.

### 1. Developer Console Mode (Standard Workstation Flow)

By default, the development configuration (`-DAHC_DEV_MODE=ON`) maps your interactive app to a standard console-attached layout so that `SDL_Log` outputs stream cleanly to your physical terminal window in real-time.

```bash
# A. Configure the workspace layout with Dev Console parameters active
cmake -S . -B Build -DAHC_DEV_MODE=ON

# B. Compile all core library layers and executable binaries
cmake --build Build

```

#### Executing Your Targets:

Once compiled, you can run either target straight through your shell terminal session:

* **To execute the Primary Emulator App (Amstrad-Blue Window):**
```bash
./Build/AHCPCPEMUAPP

```


* **To execute the Text-Only Diagnostic Engine (Continuous Integration Suite):**
```bash
./Build/AHCPCPEMU

```



---

### 2. Retail App Bundle Deployment Mode

When you want to bundle your emulator into a standalone, native macOS **`.app` bundle package** (which isolates files, sets custom app identifiers like `com.akashatek.ahcepcpemu`, and encapsulates asset tracking pathways), switch development mode off:

```bash
# A. Re-configure the build matrix to construct an Apple App Bundle
cmake -S . -B Build -DAHC_DEV_MODE=OFF

# B. Run the unified target recompilation sequence
cmake --build Build

# C. Launch your compiled standalone deployment layer natively
open Build/AHCPCPEMUAPP.app

```

---

## 🧹 Housekeeping & Project Cleanup

If you want to flush away your intermediate compiler files, object arrays, or target execution builds without wiping out your structural cache configurations, call the native CMake cleaning wrapper:

```bash
# Resets compiled binaries while leaving generation infrastructure completely intact
cmake --build Build --target clean

```

If you want to perform a completely pristine "nuclear" reset of your generation workspace to troubleshoot changes made to `CMakeLists.txt`, safely flush out the `Build/` directory parameters like this:

```bash
# Deletes temporary local build configurations without losing the directory
rm -rf Build/*

```

---

## 📝 Hardware Architecture Notes

* **CPU Timing Matrix:** The system framework targets a simulated `4.0 MHz` instruction step block configuration.
* **Graphical Canvas Engine:** Interactive viewports run at native hardware refresh rates mapped over modern GPU backend structures bound to an unextended C++20 design configuration.

```

This updates your documentation completely and ensures anyone pulling your repository knows exactly how to step between the **Headless Runner** (`AHCPCPEMU`) and the **Graphical Application** (`AHCPCPEMUAPP`)!

```