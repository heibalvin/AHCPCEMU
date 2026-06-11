# AHCPCPEMU — Amstrad CPC Architecture Framework

A modern, multi-platform Amstrad CPC hardware emulator backend core and GUI shell wrapper designed using C++20 and SDL3.

This repository features a decoupled architecture split into a shared core system library (`CPCEMUCORE`), an interactive graphical application (`AHCPCPEMUAPP`), and an automated headless verification test execution runner (`AHCPCPEMU`).

---

       ┌────────────────────────────────────────────────────────┐
       │                 CPC_APP (SDL3 Host Layer)              │
       └───────────────────────────┬────────────────────────────┘
                                   │ Owns / Drives
                                   ▼
       ┌────────────────────────────────────────────────────────┐
       │             CPC_EMU (Master Core Container)            │
       └─────┬──────────────┬──────────────┬──────────────┬─────┘
             │              │              │              │
             ▼              ▼              ▼              ▼
       ┌───────────┐  ┌───────────┐  ┌───────────┐  ┌───────────┐
       │  CPC_CPU  │  │ CPC_VDP   │  │  CPC_DSK  │  │  CPC_MMI  │
       └─────┬─────┘  └─────┬─────┘  └─────┬─────┘  └─────┬─────┘
             │              │              │              │
             └──────────────┴──────┬───────┴──────────────┘
                                   │ All Communicate Via
                                   ▼
       ┌────────────────────────────────────────────────────────┐
       │                        CPC_BUS                         │
       └────────────────────────────────────────────────────────┘

| Subsystem Tag | Domain Name | Core Engineering Responsibility |
| :--- | :--- | :--- |
| **`CPC_APP`** | Graphical Host Wrapper | The SDL3 host app framework (`AHCPCPEMUAPP`) handling your physical window, audio backends, and render targets. |
| **`CPC_EMU`** | Master Core Orchestrator | The container component inside `CPCEMUCORE`. Ticks the clock cycles and syncs the CPU, CRTC, and BUS together. |
| **`CPC_BUS`** | System Interconnect Bus | The master communication backbone. Routes memory reads/writes, I/O mapping, and coordinates components. |
| **`CPC_CPU`** | Central Processing Unit | Emulates Z80 instruction cycle fetching, execution, registers, and timing hooks. |
| **`CPC_VDP`** | Video Display Processor | Handles display timing, sync signals, and video RAM address generation (Motorola 6845). |
| **`CPC_DSK`** | Disk Controller & Storage | Parses sector geometries of `.DSK` raw images, emulating the NEC uPD765 Floppy Disk Controller. |
| **`CPC_MMI`** | Man-Machine Interface | Handles translation of physical host keyboard/joystick events down into virtual peripheral matrix lines. |
---

## 📦 Directory Structure

```text
AHCPCPEMU/
├── CMakeLists.txt        # Master multi-platform build specification script
├── Makefile              # Quality-of-life automation shortcuts for terminal
├── README.md             # Project roadmap and framework guidelines
├── .gitignore            # Version control tracking constraints
├── Resources/            # Embedded hardware system binaries
│   ├── OS_6128.ROM
│   └── BASIC_6128.ROM
├── Include/              # Object interface definition headers (.h)
│   ├── cpcapp.h          # Host OS graphical layer coordinator
│   ├── cpccommon.h       # Shared subcategories, macros, and logging utilities
│   └── cpcemu.h          # Virtual motherboard/bus hardware core orchestrator
└── Source/               # Implementation files (.cpp)
    ├── main.cpp          # Headless diagnostic app entry point
    ├── main_app.cpp      # Graphical windowed app entry point
    ├── cpcapp.cpp        # Window/Event lifecycle routines
    ├── cpccommon.cpp     # SDL-compliant zero-allocation logging callback
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

## 🛠️ Build Automation & Execution Guide

This repository utilizes a dual-tier build system: **CMake** serves as the cross-platform meta-generator, while a root-level **Makefile** acts as a convenience shortcut layer to automate compiling, cleaning, and execution right from your terminal window.

```text
 Terminal Command ──► Makefile Shortcut ──► CMake Configuration ──► Compiler Target Output

```

### 📋 Command Reference Matrix

| Command | Action Type | Primary Responsibility |
| --- | --- | --- |
| **`make`** or **`make dev`** | **Build** | Generates build recipes and compiles both app targets with the development console enabled (`AHC_DEV_MODE=ON`). |
| **`make retail`** | **Build** | Generates build recipes and compiles a production-ready, standalone macOS App bundle without terminal attachment (`AHC_DEV_MODE=OFF`). |
| **`make run`** | **Execute** | Automatically updates modified code targets, then runs the graphical interactive emulator wrapper (**`AHCPCPEMUAPP`**). |
| **`make run-headless`** | **Execute** | Automatically updates modified code targets, then runs the headless automated test suite (**`AHCPCPEMU`**). |
| **`make clean`** | **Maintenance** | Removes compiled object (`.o`) files and binaries, leaving the primary CMake system generation configuration cache intact. |
| **`make superclean`** | **Maintenance** | **"Nuclear option."** Completely purges the entire `Build/` directory cache, resetting your compilation environment to zero. |

---

### ⚡ Detailed Workflow Implementations

#### 1. Incremental Compilation (`make`)

Compiles the structural static core library `CPCEMUCORE` along with your active executable applications. Subsequent calls only rebuild files you have altered, speeding up execution.

```bash
make

```

#### 2. Running the Interactive Graphical App (`make run`)

Invokes a dependency check to ensure your local source changes are compiled, then starts the SDL3 hardware-accelerated viewport display. All engine output layers route cleanly to your terminal.

```bash
make run

```

#### 3. Running Headless Diagnostic Suites (`make run-headless`)

Launches the engine runner bypassing window allocation completely. This is perfect for verifying CPU instruction cycles, BUS layouts, or parsing `.DSK` track sectors directly in pure terminal conditions.

```bash
make run-headless

```

#### 4. Clearing Environment Faults (`make superclean`)

If you modify target rules inside `CMakeLists.txt`, change structural system variables, or run into compiler sync glitches on macOS, completely flush your environment before building fresh:

```bash
make superclean
make run

```