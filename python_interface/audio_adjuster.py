import serial
import string

from audio_interface import Audio_Int

# Run and wait for serial input to increase vol
# Run as constant process

speakers = Audio_Int()
print( speakers.get_vol() )

#output = ""
#ser = serial.Serial("/dev/ttUSB0", 9600, 8, 'N', 1, timeout=1)
#while output != " ":
    #output = ser.readline()
    #print(output)