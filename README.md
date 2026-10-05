# tf2vr-timefix

> [!IMPORTANT]
> This has been merged into Titanfall 2 VR as of v1.0.13. You should no longer need this.

Quick and dirty workaround for the "hand motion requires an input timestamp" crash when throwing grenades in Titanfall 2 VR on the HP Reverb G2.  
This should work for other WMR headsets as well, given they're also affected.

## Usage

Download the DLL file from [the Release page](https://github.com/geniiii/tf2vr-timefix/releases/latest) and place it in `steamapps\common\Titanfall2\TF2VR\plugins\`.

Upon launching the game, you should see a `tf2vr_timefix.log` file next to the DLL. You can open the log file to see if the plugin is doing anything.

## Building

1. Install Visual Studio 2019 or newer with the C++ development tools
2. Run `build.bat`
