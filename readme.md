# USIS Protocol

The **Universal Spectroscope Interface Specification (USIS) protocol** is designed to control any astronomical spectroscope for remote operation. This protocol is proposed by Shelyak Instruments company (www.shelyak.com). 

Using a standard protocol makes possible to any device with any software. It helps the developers (both hardware and software) with precise guidelines.

In this repository, you will find:

- the specification itself ([USIS-specification.md](USIS-specification.md)),
- a Library that implements the USIS protocol for Arduino environment (can be used for Atmega or Raspberry-Pi-Pico micro-controllers). Based on the library, you can develop your own code for a new spectroscopy device.
- Some basic demo examples
- A demo .uf2 file that you can drag and drop in a Raspberry Pi Pico to get a basic USIS device (useful for development).



## Usis library 

How to install Usis library on your computer

### On Windows

just copy this folder under `Documents/Arduino/libaries/usis`

open the Arduino IDE (if it was opened, you will have to restart it)

open usis example `File/examples/usis/test-01`

And run !
