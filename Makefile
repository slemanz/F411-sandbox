MAKEFLAGS  += --no-print-directory
BUILD_DIR   = build
TOOLCHAIN   = cmake/arm-none-eabi-gcc.cmake

all: $(BUILD_DIR)/Makefile
	cmake --build $(BUILD_DIR)

$(BUILD_DIR)/Makefile:
	cmake -B $(BUILD_DIR) -DCMAKE_TOOLCHAIN_FILE=$(TOOLCHAIN)

load: all
	cmake --build $(BUILD_DIR) --target load

clean:
	rm -rf $(BUILD_DIR)
