from print_helpers import *
from audio_interface import AudioIntfc

''' Class to handle requests from the ESP32 '''
class MyESP32CmdHandler:
    speaker_intfc = None

    ''' Expected message plus new line to escape readline '''
    EXPECTED_DATA_SIZE  = 4
    ''' Expected header for call commmands '''
    EXPECTED_HDR        = (0x55, 0x54)
    ''' Expected command bytes '''
    VOL_UP_CMD          = 0x01
    VOL_DOWN_CMD        = 0x02
    VOL_MUTE_CMD        = 0x03

    def __init__(self):
        self.speaker_intfc = AudioIntfc()

    def hdrIsValid(self, input_w_header):
        if input_w_header[0] == self.EXPECTED_HDR[0]:
            if input_w_header[1] == self.EXPECTED_HDR[1]:
                return True
        return False

    def handleSerialLine(self, serial_input):
        if len(serial_input) != self.EXPECTED_DATA_SIZE:
            printFWoNl("Received non-cmd data, len", len(serial_input))
            printFWoNl( ":", serial_input.decode("utf-8"))
            return

        if self.hdrIsValid(serial_input) == False:
            print("Invalid header: ", hex(serial_input[0]), hex(serial_input[1]) )

        cmd = serial_input[2]
        if cmd == self.VOL_UP_CMD:
            printF("Increasing volume")
            self.speaker_intfc.increaseVolume()
        elif cmd == self.VOL_DOWN_CMD:
            printF( "Decreasing volume")
            self.speaker_intfc.decreaseVolume()
        elif cmd == self.VOL_MUTE_CMD:
            printF( "Toggling mute")
            self.speaker_intfc.toggleMute()
        else:
            printFWoNl("Valid header, invalid cmd. Data: ")
            for char in serial_input:
                printFWoNl( hex(char))
            printFWoNl("")