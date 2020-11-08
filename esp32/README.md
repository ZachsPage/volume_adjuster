# Classic Bluetooth to Serial
Starts a FreeRTOS task to wait for a Classic Bluetooth connection, read BT data,
and transmit validated data over serial.

## TODO
* Legacy pin is not working 
    * esp_files/sdconfig.defaults issue
    * May need to update IDF

## ESP IDF Build System Changes
* Modified the build system to not hide as much functionality
* Changed CMakelist to only build minimum components

## Building Requirements / Steps
* Setup the toolchain from [here](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/linux-setup.html)
* Then, follow the guide [here](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/), up until it directs on how to build "hello-world"
* After downloading the idf, check you have permissions to write.
    * If you have errors dealing with "configure_file", it is because you cannot
    copy files in that directory; either fix permissions or run `sudo make all`
    instead (not advised)
* Originally, the esp-idf and this project were built using an Ubuntu VM with 
a shared Windows drive. There was build issues relating to CMake's "configure file" 
even though the permissions appeared correct. Now, it is suggested to move the
project's files onto Linux's drive
* `make all` in this directory

## Flashing
* To see the serial:
    * Make a 115200 Putty profile. Name it "ESP_32"
    * `sudo putty -load ESP_32`
* Disconnect session to flash:
    * `make flashAll` or if just the app has changed `make flashApp`

## Relevant Documentation
* https://en.wikipedia.org/wiki/List_of_Bluetooth_profiles
    * Why BT Profile SPP (Serial Port Profile) was selected