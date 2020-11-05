from esp32_cmd_handler import MyESP32CmdHandler
from user_serial import MySerial

''' Sets up bridge between serial port to ESP32 command handler'''
def main():
    esp32_serial_handler = MyESP32CmdHandler()
    my_serial = MySerial(esp32_serial_handler)
    my_serial.start_serial_listener()

if __name__ == "__main__":
    main()