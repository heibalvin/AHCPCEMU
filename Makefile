# ==============================================================================
# AHCPCPEMU Automation Wrapper Shortcuts
# ==============================================================================

.PHONY: all dev retail clean superclean run run-headless

# Default target: builds everything in development mode
all: dev

# Configure and build with development console enabled
dev:
	cmake -S . -B Build -DAHC_DEV_MODE=ON
	cmake --build Build

# Configure and build as a pure macOS standalone application bundle
retail:
	cmake -S . -B Build -DAHC_DEV_MODE=OFF
	cmake --build Build

# Run the graphical app (Attached to the terminal so you can see your custom logs)
run: dev
	./Build/AHCPCPEMUAPP

# Run the headless diagnostic suite
run-headless: dev
	./Build/AHCPCPEMU

# Standard object/binary clean (Leaves CMake cache intact)
clean:
	cmake --build Build --target clean

# "Nuclear" clean: Completely purges the Build folder cache
superclean:
	rm -rf Build/* Build/.cmake