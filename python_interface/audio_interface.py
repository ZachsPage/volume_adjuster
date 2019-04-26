from ctypes import cast, POINTER
from comtypes import CLSCTX_ALL
from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume

class Audio_Int:
    last_volume = 0
    devices = 0
    interface = 0
    speakers = 0

    def updateVol(self):
        self.last_volume = self.speakers.GetMasterVolumeLevel()

    def __init__(self):
        self.devices = AudioUtilities.GetSpeakers()
        self.interface = self.devices.Activate(IAudioEndpointVolume._iid_, CLSCTX_ALL, None)
        self.speakers = cast(self.interface, POINTER(IAudioEndpointVolume))
        self.updateVol()
    
    def get_vol(self):
        return self.speakers.GetMasterVolumeLevel()
        
    def inc_vol(self, amount):
        currVol = self.speakers.GetMasterVolumeLevel()
        self.speakers.SetMasterVolumeLevel(currVol+(2 * amount), None)
        self.updateVol()

    def dec_vol(self, amount):
        currVol = self.speakers.GetMasterVolumeLevel()
        self.speakers.SetMasterVolumeLevel(currVol-(2 * amount), None)
        self.updateVol()

    def mute(self):
        currVol = self.speakers.GetMasterVolumeLevel()
        self.speakers.SetMasterVolumeLevel(-62, None)

    def unmute(self):
        self.speakers.SetMasterVolumeLevel(self.last_volume, None)

    #100 is 0. 
    # 90 is -1.576469898223877. 
    # 80 is -3.335562229156494. 
    # 50 is -10.312137603759766. 
    # 30 is -17.78333282470703
    # 0 is -62
    #currRange = volume.GetVolumeRange()
    #print(currRange[0]) # 0... -64?
    #print(currRange[1]) # 100... 0