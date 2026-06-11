# ==============================================================================
# AHCPCPEMU UNIFIED AUTOMATION SHORTCUTS
# ==============================================================================

.PHONY: all build clean run run-headless

# Default action is to compile the single master binary
all: build

build:
	cmake -S . -B Build
	cmake --build Build

# Safe cleanup: Clears out binaries, objects, and configurations 
# while explicitly leaving the Build directory standing empty.
clean:
	rm -rf Build/*
	@echo "Build artifacts purged. Storage directory structure preserved."

# Launches the interactive GUI Window mode
run: build
	./Build/CPCEMU

# Launches the pure console headless mode (No window)
run-headless: build
	./Build/CPCEMU --headless