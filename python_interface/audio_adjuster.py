import serial
import string

from audio_interface import Audio_Int

if __name__ == "__main__":
    main()

def main():
    print( "Getting audio interface to manipulate volume for..." )
    speakers = Audio_Int()

    print( "Service started! Listening for ESP32 serial input..." )
    SERIAL_PORT = "/dev/ttyUSB0"
    SERIAL_BAUD = 9600
    SERIAL_BYTE_SIZE = serial.EIGHTBITS
    SERIAL_PARITY = serial.PARITY_NONE
    SERIAL_STOPBITS = serial.STOPBITS_ONE
    SERIAL_TIMEOUT_SECS = None # Blocking
    serial_port = serial.Serial(SERIAL_PORT, SERIAL_BAUD, SERIAL_BYTE_SIZE,
                                SERIAL_PARTIY, SERIAL_STOPBITS, SERIAL_TIMEOUT_SECS)
    while true:
        output = serial_port.readline()
        print( "Received serial input... " )
        for char in output:
            hex_char = int(char.encode('hex'), 16)
            print( hex(hex_char), end=' ' )
        print( "End data\n" )