# Python Volume Adjuster
pycaw requires Microsoft Visual C++ 14.0
* To get this without install all of Visual Studio, Google for:
    * `microsoft visual c++ 2015 build tools` - download / install
    * NOTE: This is different than the `microsoft build tools`, and is ~6gb
        * Even if you have Windows 10, the Windows 8.1 SDK can be used (saves ~2gb)
    * TODO: Rewrite this to not require this package... but likely some form
      of a Windows SDK is required to interface with the audio manager...