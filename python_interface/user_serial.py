import serial
import serial.tools.list_ports
from print_helpers import *

''' Class to open/use serial port using specified attributes & user input '''
''' - TODO: Force - dep. inj. serial_handler w/ handleSerialLine(input) '''
class MySerial:
    SERIAL_BAUD = 115200
    SERIAL_BYTE_SIZE = serial.EIGHTBITS
    SERIAL_PARITY = serial.PARITY_NONE
    SERIAL_STOPBITS = serial.STOPBITS_ONE
    SERIAL_TIMEOUT_SECS = None # Make port blocking

    user_serial_name = ""
    serial_port = None
    serial_handler = None

    def __init__(self, serial_handler):
        serial_list = serial.tools.list_ports.comports()
        num_serial_devices = len( serial_list )
        if num_serial_devices == 0:
            print("No serial devices available - please connect device")
            quit()

        user_input_valid = False
        serial_port_name = ""
        while( user_input_valid == False ):
            print("Select Windows COM port for desire serial device...")
            i = 0
            for comm_port in serial_list:
                print("Device ", i, ": ", comm_port.device)
                i = i+1
            
            try:
                user_selected_idx = input("Enter device (not COM) number to use: ")
                user_selected_idx = int( user_selected_idx )
            except:
                print("User intput", user_selected_idx, "not valid")
                quit() 

            if user_selected_idx >= num_serial_devices:
                print("Invalid index selection... ")
            else:
                user_input_valid = True
                self.user_serial_name = serial_list[ user_selected_idx ].device
                self.init_serial()
            
            self.serial_handler = serial_handler
    
    def init_serial(self):
        if not self.user_serial_name:
            print("Select valid serial port!")

        self.serial_port = serial.Serial(self.user_serial_name,
            self.SERIAL_BAUD, self.SERIAL_BYTE_SIZE, self.SERIAL_PARITY,
            self.SERIAL_STOPBITS, self.SERIAL_TIMEOUT_SECS)
        self.serial_port.reset_input_buffer()

    def start_serial_listener(self):
        if not self.serial_port or not self.serial_handler:
            print("User serial not init!")
            return
        # Read until we get a \n - size will include the newline...
        serial_input = "" 
        while True:
            serial_input = self.serial_port.readline()
            self.serial_handler.handleSerialLine(serial_input)