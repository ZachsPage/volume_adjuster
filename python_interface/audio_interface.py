from ctypes import cast, POINTER
from comtypes import CLSCTX_ALL
from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume

''' Class to grab the audio interface on Windows '''
class Audio_Int:
    ''' Track last volume for unmute '''
    last_volume = 0
    ''' Used to get the speakers init '''
    devices = 0
    ''' Used to get the speakers at init ''' 
    interface = 0
    ''' The actual speaker object to manipulate '''
    speakers = 0
    ''' Value to set speakers actual volume to 0 '''
    ''' - Found with currRange = volume.GetVolumeRange() print currRange [0]''' 
    MUTE_VALUE = -62
    ''' Value to set speakers actual volume to 100 '''
    ''' - Found with currRange = volume.GetVolumeRange() print currRange [1]''' 
    ''' - Volume of 90 is -1.576, 80 is -3.334, 50 is -10.312, 30 is -17.783...'''
    MAX_VOL_VALUE = 0

    def __init__(self):
        self.devices = AudioUtilities.GetSpeakers()
        self.interface = self.devices.Activate(IAudioEndpointVolume._iid_, CLSCTX_ALL, None)
        self.speakers = cast(self.interface, POINTER(IAudioEndpointVolume))
        self.updateLastVolume()
    
    def getVolume(self):
        return self.speakers.GetMasterVolumeLevel()

    def updateLastVolume(self):
        self.last_volume = self.speakers.GetMasterVolumeLevel()
        
    def increaseVolume(self, amount):
        curr_vol = self.speakers.GetMasterVolumeLevel()
        self.speakers.SetMasterVolumeLevel(currVol+(2 * amount), None)
        self.updateLastVolume()

    def decreaseVolume(self, amount):
        curr_vol = self.speakers.GetMasterVolumeLevel()
        self.speakers.SetMasterVolumeLevel(currVol-(2 * amount), None)
        self.updateLastVolume()

    def mute(self):
        self.updateLastVolume()
        self.speakers.SetMasterVolumeLevel(MUTE_VALUE, None)

    def unmute(self):
        self.speakers.SetMasterVolumeLevel(self.last_volume, None)