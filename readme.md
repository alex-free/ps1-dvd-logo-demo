# [alex-free.github.io](https://alex-free.github.io)

# PS1 DVD Logo Demo

_By Alex Free_

PS1 DVD Logo Demo is a small demo for the PlayStation 1 console. It essentially demonstrates the following features/functionality:

*   Bounces a large 128x74 DVD logo sprite around a screen (of 320x240p resolution) at 60 FPS.
*   The DVD logo sprite can randomly change to 1 of 7 different colors each time the logo bounces off of the screen and changes direction.
*   When an exact symmetrical corner hit occurs, an 'exact corner hit counter' will appear at the top of the screen after a very brief screen flash of white, as well as a message explaining the controls to dismiss said message. Press `DPAD-UP` on a controller to dismiss it until the next exact symmetrical corner hit occurs, or press `DPAD-DOWN` to dismiss it forever. The message explaining the controls will automatically disappear in about 30 seconds after first being displayed if you do nothing. If you do press up or down on your controller, the message explaining the controls will disappear along with the 'exact corner hit counter'.

PS1 DVD Player uses 7 different CLUTs (ColorLookUpTables) to change the appearance of the single DVD logo sprite, using barely any VRAM. This demo also does not use the `libgs` library, making it "more to the metal" and tiny in size. The HBlank root counter is used to seed the random function for selecting each CLUT/color on screen.

**All Japanese and American PS1/PS2 consoles are supported** as they have the NTSC video mode. Everything is currently hard-coded to NTSC 320x240 resolution, and the demo will currently not work on PAL consoles in PAL video mode. PAL consoles with an NTSC video mod would also work.

## External Links

*   [PS1 DVD Logo Demo Homepage](https://alex-free.github.io/ps1-dvd-logo-demo)
*   [PS1 DVD Logo Demo GitHub](https://github.com/alex-free/ps1-dvd-logo-demo)
*   [Tonyhax International](https://alex-free.github.io/tonyhax-international) - A recommended boot method to start the demo on any stock console without required a mod chip to be installed.
*   [Stealth ROM](http://www.psxdev.net/forum/viewtopic.php?f=66&t=3966) - A recommended boot method to start the demo on SCPH-1000-SCPH-7502 stock consoles without require a mod chip to be installed.

## Downloads

### Version 1.0 (11/25/2022)

*   [PS1 DVD Logo Demo](https://github.com/alex-free/ps1-dvd-logo-demo/releases/download/v1.0/ps1-dvd-logo-demo-build-1.0.zip) _BIN+CUE CD image and PS-EXE_
*   [Source](https://github.com/alex-free/ps1-dvd-logo-demo/archive/refs/tags/v1.0.zip)

## Building From Source

Requirements

*   [PSN00BSDK](https://github.com/LameGuy64/psn00bsdk)
*   [MKPSXISO](https://github.com/LameGuy64/mkpsxiso)
*   [*My TimEdit Fork](https://github.com/alex-free/timedit)
*   [CD License File: licensej.dat](https://github.com/alex-free/tonyhax/raw/master/boot-cd/licensej.dat) - this must be in the `ps1-dvd-logo-demo` source directory. 

*_Timedit is optional and not required for just building the source. However if you want to mess with the TimEdit project file `dvd.tpj` to change the TIM image. I Suggest my fork because currently (as of 11/25/2022) the [original TimEdit] doesn't work on Linux, only Windows. My fork works on both._


After [downloading](#downloads) the latest source, `cd` into `ps1-dvd-logo-demo` source directory and execute these commands (this is a one-liner):

`cmake --preset default . && cmake --build ./build --clean-first`

## License

PS1 DVD Logo Demo is released under the 3-BSD license. See the file `license.txt` in each release for more info.

## Credits

*   [LameGuy64](https://lameguy64.net) for [PSN00BSDK](https://github.com/LameGuy64/psn00bsdk), [TimEdit](https://github.com/alex-free/LameGuy64/timedit), [MKPSXISO](https://github.com/LameGuy64/mkpsxiso), and the [Lameguy's PlayStation Programming Series](http://lameguy64.net/tutorials/pstutorials/). All of which where used to make the demo. I did make a [fork](https://github.com/alex-free/timedit) of TimEdit so that I could compile and use it natively on Linux however.
*   [SpicyJPEG](https://github.com/spicyjpeg) for guiding me in the right direction when I encountered a [PSN00BSDK bug](https://github.com/Lameguy64/PSn00bSDK/issues/66).
*   [Pngfind.com](https://www.pngfind.com/) for the [DVD logo PNG](https://www.pngfind.com/mpng/hmTRwb_white-dvd-logo-png-sketch-transparent-png/) edited in TimEdit.