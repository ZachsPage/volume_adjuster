from ctypes import cast, POINTER
from comtypes import CLSCTX_ALL
from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume

''' Class to control the audio interface on Windows '''
class AudioIntfc:
    ''' Track last volume for unmute '''
    last_volume = 0
    ''' Used to get the speakers init '''
    devices = 0
    ''' Used to get the speakers at init ''' 
    interface = 0
    ''' The actual speaker object to manipulate '''
    speakers = 0
    ''' Track if the speakers have been toggled to mute '''
    muted = False
    ''' Value to set speakers actual volume to 0 '''
    ''' - Found with currRange = volume.GetVolumeRange() print currRange [0]''' 
    MUTE_VALUE = -62
    ''' Value to set speakers actual volume to 100 '''
    ''' - Found with currRange = volume.GetVolumeRange() print currRange [1]''' 
    ''' - Volume of 90 is -1.576, 80 is -3.334, 50 is -10.312, 30 is -17.783...'''
    MAX_VOL_VALUE = 0
    ''' Volume adjust per increment/decrement - see doc for MAX_VOL_VALUE '''
    VOL_OFFSET = 2

    def __init__(self):
        self.devices = AudioUtilities.GetSpeakers()
        self.interface = self.devices.Activate(IAudioEndpointVolume._iid_, CLSCTX_ALL, None)
        self.speakers = cast(self.interface, POINTER(IAudioEndpointVolume))
        self.updateLastVolume()
    
    def getVolume(self):
        return self.speakers.GetMasterVolumeLevel()

    def updateLastVolume(self):
        self.last_volume = self.speakers.GetMasterVolumeLevel()
        
    def increaseVolume(self):
        curr_vol = self.speakers.GetMasterVolumeLevel()
        new_vol = min(curr_vol+self.VOL_OFFSET, self.MAX_VOL_VALUE)
        self.speakers.SetMasterVolumeLevel(new_vol, None)
        self.updateLastVolume()

    def decreaseVolume(self):
        curr_vol = self.speakers.GetMasterVolumeLevel()
        new_vol = max(curr_vol-self.VOL_OFFSET, self.MUTE_VALUE)
        self.speakers.SetMasterVolumeLevel(new_vol, None)
        self.updateLastVolume()
    
    def toggleMute(self):
        if not self.muted:
            self.mute()
        else:
            self.unmute()

    def mute(self):
        self.updateLastVolume()
        self.speakers.SetMasterVolumeLevel(self.MUTE_VALUE, None)
        self.muted = True

    def unmute(self):
        self.speakers.SetMasterVolumeLevel(self.last_volume, None)
        self.muted = False