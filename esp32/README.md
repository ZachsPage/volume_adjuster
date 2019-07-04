# Classic Bluetooth to Serial
Starts a FreeRTOS task to wait for a Classic Bluetooth connection, read BT data, and transmit some of the data over serial.

## What Needs Done
Currently crashing with some MAC address corruption that is stored in BLK0, not allowing the BT controller to be enabled

## Build System Changes
* Modified the build system to not hide as much functionality
* Changed CMakelist to only build minimum components

## Requirements
* Setup the toolchain from [here](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/linux-setup.html)
* Then, follow the guide [here](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/), up until it directs on how to build "hello-world"
* After downloading the idf, check you have permissions to write. If you have errors dealing with "configure_file", it is because you cannot copy files in that directory; either fix permissions or run `sudo make all` instead (not advised)