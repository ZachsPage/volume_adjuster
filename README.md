# Android -> ESP32 -> PC Volume Adjuster
## Description
The goal of this repo is to allow adjustment of a Window's machines volume from
an Android phone. The simple way would be directly over the network, but lets
throw an ESP32 in there for fun.

The flow is an Android application -> SPP Bluetooth -> ESP32 -> Serial -> Python
* See the top level diagram below
![Top Level Diagram](images/top_level.png)

## Building
* Originally, the esp-idf and this project were built using an Ubuntu VM with 
a shared Windows drive. There was build issues relating to CMake's "configure file" 
even though the permissions appeared correct. Now, it is suggested to move the
project's files onto Linux's drive
* Once setup, build this ESP project using `make all`
