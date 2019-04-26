# REQUIREMENTS
# Export IDF_PATH to the github ESP IDF clone path
# Esport ESP_FLASH to the github esptool clone path

PROJECT_NAME := hello-world

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

cmake: 
	mkdir -p build
	cd build && cmake ../ && make
	rm sdkconfig

flashMessage:
	@echo "\n> When 'Connecting......' appears, a board button may need pressed until the flash starts\n"

appFlash: flashMessage
	$(FLASH) $(APP)

bootFlash: flashMessage
	$(FLASH) $(BOOTLOADER)

partFlash: flashMessage
	$(FLASH) $(PART_TABLE)

allFlash: flashMessage
	$(FLASH) $(BOOTLOADER) $(PART_TABLE) $(APP)

genPart:
	$(PART_GEN) $(PART_NAME).csv build/$(PART_NAME).bin	

all: cmake genPart

clean:
	rm -r build/*
