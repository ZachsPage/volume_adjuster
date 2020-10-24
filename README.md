# Android->ESP32->PC Volume Adjuster
## DESCRIPTION
-----------

## Building
* Originally, the esp-idf and this project were built using an Ubuntu VM with 
a shared Windows drive. There was build issues relating to CMake's "configure file" 
even though the permissions appeared correct. Now, it is suggested to move the
project's files onto Linux's drive
* Once setup, build this ESP project using `make all`

## Top Level Diagram
-----------------
![Top Level Diagram](images/top_level.png)
