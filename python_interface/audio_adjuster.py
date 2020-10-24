import serial
import serial.tools.list_ports
import string

from audio_interface import Audio_Int

def main():
    print( "Getting audio interface to manipulate volume for..." )
    speakers = Audio_Int()
    print("Started! Getting list of available serial devices...")
    serial_list = serial.tools.list_ports.comports()
    num_serial_devices = len( serial_list )
    if num_serial_devices == 0:
        print("No serial devices available - please connect ESP32")
        quit()

    user_input_valid = False
    serial_port_name = ""
    while( user_input_valid == False ):
        print("Select the Windows COM port the ESP is connected to...")
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
            serial_port_name = serial_list[ user_selected_idx ].device

    print( "Listening for ESP32 serial input on:", serial_port_name, flush=True)
    SERIAL_BAUD = 115200
    SERIAL_BYTE_SIZE = serial.EIGHTBITS
    SERIAL_PARITY = serial.PARITY_NONE
    SERIAL_STOPBITS = serial.STOPBITS_ONE
    SERIAL_TIMEOUT_SECS = None # Make port blocking
    serial_port = serial.Serial(serial_port_name, SERIAL_BAUD, SERIAL_BYTE_SIZE,
                                SERIAL_PARITY, SERIAL_STOPBITS, SERIAL_TIMEOUT_SECS)
    serial_port.reset_input_buffer()
    while True:
        #Read until we get a \n - size will include the newline...
        output = serial_port.readline()
        print( "Received serial input... " )
        for char in output:
            print( hex(char) )
        print( "End data, sized ", len(output), flush=True )

if __name__ == "__main__":
    main()