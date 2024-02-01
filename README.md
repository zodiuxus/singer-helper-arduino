# Singer Helper

### better name to be found...

Small Arduino project that helps you meet a certain note with your voice with the help of FFT.

## Components and schematic
I might revisit this project in the future (as with any other of my projects), so this list may or may not change.
- Arduino NANO
- 2 push buttons
- MAX4466 microphone
- 32x8 LED matrix (or less, but you would have to change the code for your needs)

![](https://i.imgur.com/moi17Gm.png)

## Installation and usage
There are 2 .ino files, one being the main program, and the other for testing your microphone's capabilities.

To use the main program, simply download [main.ino](main.ino), then:
1. Install [Adafruit NeoMatrix](https://github.com/adafruit/Adafruit_NeoMatrix) (and its dependencies), and [arduinoFFT](https://github.com/kosme/arduinoFFT)
2. Connect everything as per the schematic and upload (this was done on a NANO clone so you don't have to worry about memory issues)

Afterwards, you can plug your Arduino into a 5V supply (like a power bank) and watch it as it tries to get you to correct your pitch.

## Issues
I have had an issue with my MAX4466 (though I believe it to be a Nano issue) where the microphone would simply not be able to pick up any sound, but instead only has hum. I've tested this numerous times, but to no avail, it's nothing but buzzing. There is no shorting between the terminals on the microphone (and I resoldered them at least 5 times), but when I plug it in, suddenly the short circuit is there (between GND and OUT).

Some inspiration (and especially the calculation sheet) taken from s-marley's repository [here](https://github.com/s-marley/ESP32_FFT_VU).
