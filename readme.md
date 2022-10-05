# USIS Protocol

The **Universal Spectroscope Interface Specification (USIS) protocol** is designed to control any astronomical spectroscope for remote operation. This protocol is proposed by Shelyak Instruments company (www.shelyak.com). 

Using a standard protocol makes possible to any device with any software. It helps the developers (both hardware and software) with precise guidelines.

In this repository, you will find:

- the specification itself ([USIS-specification.md](USIS-specification.md)),
- a Library that implements the USIS protocol for Arduino environment (can be used for Atmega or Raspberry-Pi-Pico micro-controllers). Based on the library, you can develop your own code for a new spectroscopy device.
- Some basic demo examples
- A demo .uf2 file that you can drag and drop in a Raspberry Pi Pico to get a basic USIS device (useful for development).

## USIS library & demo code

 The instructions below works for **Windows, MacOS** and **Linux**

If you want to compile for the Raspberry Pi Pico, with the Arduino IDE (any other IDE is possible, specially VS Code), you can follow this link: https://arduino-pico.readthedocs.io/en/latest/

Then, just copy this folder under `Documents/Arduino/libaries/usis`

Open the Arduino IDE (if it was opened, you will have to restart it)

Open usis example `File/examples/usis/test-01`

And run !

## USIS demo code for Raspberry Pi Pico

To help you during development steps, we provide the above demo code test-01.ino and test-02.ino. From the Arduino IDE, you can compile and upload the code to a fresh raspberry Pi Pico micro-controller. This makes your Pico a "USIS device" in few clicks.

You can also generate an .uf2 file that you can upload directly to the Pico. Here is the procedure (standard Pico procedure) :

- Click on "Booster button" while connecting the Pico to your computer with a USB cable.
- This makes appear the Pico in your File Manager, under the name RPI-RP2, like any USB drive.
- Then, drag and drop the .uf2 file to this drive. This uploads the code to the Pico, and disconnect it from the File Manager.
- The Pico is ready to talk with your computer through the USIS protocol !

If you don't have the Arduino IDE, you can directly upload the test-01.ino.uf2 file available in this repo.

The test-01 example is very basic: the only available property is VERSION. Then if you send the message `GET;VERSION;VALUE`, you'll get the reply `M00;VERSION;1.0.0;;` (M00 is the error code saying all is OK). If you send any other dummy message (like `abcd`), you'll get `C02;BAD REQUEST*4C` in return.

