# Classic Bluetooth to Serial
Starts a FreeRTOS task to wait for a Classic Bluetooth connection, read BT data,
and transmit some of the data over serial.

## TODO
* Legacy pin is not working 
    * esp_files/sdconfig.defaults issue
    * May need to update IDF
    * Relying on application authentication for now

## Build System Changes
* Modified the build system to not hide as much functionality
* Changed CMakelist to only build minimum components

## Flashing
* To see the serial:
    * Make a 115200 Putty profile. Name it "ESP_32"
    * `sudo putty -load ESP_32`
* Disconnect session to flash:
    * `make flashAll` or if just the app has changed `make flashApp`

## Requirements
* Setup the toolchain from [here](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/linux-setup.html)
* Then, follow the guide [here](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/), up until it directs on how to build "hello-world"
* After downloading the idf, check you have permissions to write.
    * If you have errors dealing with "configure_file", it is because you cannot
    copy files in that directory; either fix permissions or run `sudo make all`
    instead (not advised)

## Relevant Documentation
* https://en.wikipedia.org/wiki/List_of_Bluetooth_profiles
    * Why BT Profile SPP (Serial Port Profile) was selected