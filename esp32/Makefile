# REQUIREMENTS
# Export IDF_PATH to the github ESP IDF clone path
# Export ESP_FLASH to the github esptool clone path

PROJECT_NAME := volume-adjuster

PART_NAME=part_table
PART_GEN=$(IDF_PATH)/components/partition_table/gen_esp32part.py

# Allow override of ESP USB
USB=/dev/ttyUSB0

# Part table needs flashed to CONFIG_PARTITION_TABLE_OFFSET. 
#  If changed, bootloader & partition table will need recompiled
FLASH=cd build ; $(ESP_FLASH)/esptool.py --port $(USB) --baud 115200 --before default_reset --after hard_reset write_flash -z
BOOTLOADER=0x1000 bootloader/bootloader.bin
PART_TABLE=0x8000 $(PART_NAME).bin
# Dont need multiple apps for now
APP=0x10000 $(PROJECT_NAME).bin

# Multicore builds
NPROC=$(shell nproc)

.PHONY: all cmake build flashMessage appFLash bootFlash partFlash allFlash genPart clean

all: cmake build genPart

# Redo CMake configuration - make way for new sdkconfig
cmake: 
	mkdir -p build
	rm -f sdkconfig
	cd build && cmake ../ -DPROJECT_NAME=$(PROJECT_NAME)

build:
	cd build && make -j $(NPROC)

genPart:
	$(PART_GEN) esp_files/$(PART_NAME).csv build/$(PART_NAME).bin	

clean:
	rm -rf build/*
	rm -rf sdkconfig

flashAll: flashMessage
	$(FLASH) $(BOOTLOADER) $(PART_TABLE) $(APP)

flashMessage:
	@echo "\n> When 'Connecting...' appears, press the board's button and "
	@echo "  release when the flash begins\n"

flashApp: flashMessage
	$(FLASH) $(APP)

flashBoot: flashMessage
	$(FLASH) $(BOOTLOADER)

flashPart: flashMessage
	$(FLASH) $(PART_TABLE)
