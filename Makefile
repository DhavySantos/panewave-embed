# =================================
# ESP-IDF Makefile Wrapper (Clang)
# =================================

# Path to ESP-IDF installation
IDF_PATH ?= $(HOME)/esp/esp-idf

# Default serial port and baudrate
PORT ?= /dev/ttyUSB0
BAUD ?= 115200

# Pass arguments to idf.py and ensure clang toolchain is used
IDF := idf.py -p $(PORT) -b $(BAUD) -D IDF_TOOLCHAIN=clang

# =================================
# Targets
# =================================

all: build

build:
	@$(IDF) build

flash:
	@$(IDF) flash

monitor:
	@$(IDF) monitor

flash-monitor:
	@$(IDF) flash monitor

clean:
	@$(IDF) clean

fullclean:
	@$(IDF) fullclean

erase:
	@$(IDF) erase_flash

menuconfig:
	@$(IDF) menuconfig

help:
	@echo "ESP-IDF Makefile (Clang Toolchain)"
	@echo "----------------------------------"
	@echo "make build           - Build project"
	@echo "make flash           - Flash to device"
	@echo "make monitor         - Open serial monitor"
	@echo "make flash-monitor   - Flash and monitor"
	@echo "make clean           - Clean build"
	@echo "make fullclean       - Deep clean"
	@echo "make erase           - Erase flash"
	@echo "make menuconfig      - Run menuconfig"
	@echo ""
	@echo "Variables:"
	@echo "  PORT=/dev/ttyUSBX"
	@echo "  BAUD=115200"
	@echo "  IDF_PATH=/path/to/esp-idf"

