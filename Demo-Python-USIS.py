#!/usr/bin/env python
#-*- coding: utf-8 -*-

"""
#----------------------------------------------------------------------------------
#----------------------------------------------------------------------------------
Basic script for USIS Control.
The purpose is to provide a basic Python script to offer a demo tool.
It is to be considered as an example. This is NOT designed for production.
- Original Version : F. Cochard - May 2022
#----------------------------------------------------------------------------------
#----------------------------------------------------------------------------------
"""
#----------------------------------------------------------------------------------
# Loading libraries
#----------------------------------------------------------------------------------

import cmd # for command line instructions
from threading import Lock # to manage multi-threading
import time # gère le timing (for the function sleep)
import serial # Manages the serial port
import serial.tools.list_ports # Manages the serial ports list

#----------------------------------------------------------------------------------------------
# Variables definition
#----------------------------------------------------------------------------------------------
# constants
TIMEOUT_VALUE = 3
order_err = ['Ok, order completed', 'Invalid arguments', 'Timeout reached', 'Unexpected reply', 'Serial PORT not available', 'Checksum error in the returned message']
Locker = Lock() # Allows to lock processes during variable writing

#----------------------------------------------------------------------------------------------
# Functions definition
#----------------------------------------------------------------------------------------------

def format(str):
	"Formate une commande RS232"
	a = hex(checksum(str))[2:].zfill(2).upper() # 2: to remove \x, zfill for 2 car
	chaine = str
	return chaine

def checksum(str):
	cks = 0
	stringByte = bytes(str,'utf-8')
	for car in stringByte :
		cks = cks ^ car
	return cks

def send_order(texte):
# The function returns an error code :
    trame = format(texte)
    if (SerialPortAvailable == True) :
        try: # We check that serial port is up & running
            port_serie.reset_input_buffer() # to flush the buffer		
            port_serie.write(trame.encode('ascii')) # Sends the message
            Reply_received = False # Checks if a message is received
            timeout_limit = time.time() + TIMEOUT_VALUE
            timeout_reached = False
            while (Reply_received == False and timeout_reached == False): 
                try:
                    if port_serie.in_waiting:
                        ligne = str(port_serie.readline(),'ascii')
                        Reply_received = True 
                except:
                    print("Bug with serial port - " + trame)
                    time.sleep(3) # Waits for 3 seconds before starting again
                if (time.time() >= timeout_limit) :
                    timeout_reached = True
            if (Reply_received == True) :
                return(0, ligne, trame) # Message received OK
            if (timeout_reached == True) :
                return(2, -1, trame) # error 2 = timeout
        except serial.SerialException:
            print('The USB port is not available... anymore')
            return (4, -1, trame)
    else :
        print('The USB port is not available')
        return(4, -1, trame) # Failed operation

def send_RAW_order(text):
# same as send_order, but with no formatting!
    if (SerialPortAvailable == True) :
        try: # We check that serial port is up & running
            port_serie.reset_input_buffer() 
            port_serie.reset_output_buffer() 
            port_serie.write(text.encode('ascii')) # sends the message
            port_serie.flush() # wait until all data are transmitted
            Reply_received = False # To check if message is received
            timeout_limit = time.time() + TIMEOUT_VALUE
            timeout_reached = False
            while (Reply_received == False and timeout_reached == False):
                try:
                    if port_serie.in_waiting:
                        ligne = str(port_serie.readline(),'ascii')
                        Reply_received = True # Ok, message received
                except:
                    print("Bug with serial port - " + text)
                    time.sleep(3) # Waits for 3 seconds before starting again
                if (time.time() >= timeout_limit) :
                    timeout_reached = True
            if (Reply_received == True) :
                return(0, ligne, text) # Message received OK
            if (timeout_reached == True) :
                return(2, -1, text) # error 2 = timeout
        except serial.SerialException:
            print('The USB port is not available... anymore')
            return (4, -1, text)
    else :
        print('The USB port is not available')
        return(4, -1, text) # Failed operation

def val_message(message_reply): # to extract the value from the USIS message
    MessageContent = message_reply.split(';')
    print(MessageContent)
    value = MessageContent[4].split('\n')[0] # we remove the 'line return' (\n) at the end
    error = MessageContent[0]
    return error, value


#----------------------------------------------------------------------------------------------
# End of functions definition
#----------------------------------------------------------------------------------------------

#----------------------------------------------------------------------------------------------
# Command line class definition
#----------------------------------------------------------------------------------------------

class UVEX(cmd.Cmd):
    """ This class offers a command line tool.
    Each method starting with 'do_' is a command available for the user"""
    intro = 'Welcome to UVEX control system.\nType help or ? to list commands'
    prompt = '\n> '

    # User commands 
    #----------------------------------------------------------------------------------------------
    # 1 - Commands for the whole shelter
    #----------------------------------------------------------------------------------------------

    def do_rawMessage(self, arg):
        '''to send a RAW message (with no formatting)'''
        print("Raw message", (len(arg.split(' '))))
        if (len(arg.split(' ')) == 1): # tests if there is exactly 1 argument	
            RawText = arg.split(' ')[0] + '\n'
            print(f'Send the message: {RawText}')
            reply, val, order = send_RAW_order(RawText)
            print(f'Order sent: {order}... reply: {reply} - {order_err[reply]}\n... Message returned: {val}')
        else :
            print('Requires exactly 1 argument (nb of blinks). Ex : test 3')

    def do_bye(self, arg):
        '''bye - to quit the shelter control program.'''
        if (SerialPortAvailable == True) :
            port_serie.close()
        print('End of the script.\nGood bye!')
        quit()
    def do_version(self, arg):
        '''Returns the firmware and protocol (USIS) version'''
        if (len(arg.split()) == 0): # No argument is required
            Message = 'GET;VERSION;VALUE\n'
            reply, val, order = send_order(Message)
            print(f'Order sent: {order}... reply: {reply}  - {order_err[reply]}\nMessage returned: {val}')
        else:
            print('Requires no argument')
 
#----------------------------------------------------------------------------------------------
# Main program
#----------------------------------------------------------------------------------------------

# Open the serial port (once, to avoid auto-reset)
try: # Gets the list of the USB ports
    ports = list(serial.tools.list_ports.comports())
    NbPorts = len(ports)
    print("Here is the list of the available USB ports on your computer")
    for Idx in range(0, NbPorts):
        print(f"{Idx+1} : {ports[Idx][0]}")
    Choice = int(input("Which USB port do you want to open (give the number)?\n") or "1")-1
    found_port = ports[Choice][0]
    print('Opening port :' + found_port)
    try: # To check that USB ports is available
        port_serie = serial.Serial(port=found_port, baudrate=9600, timeout=1, writeTimeout=1)
        print("The USB port is open.")
    except serial.SerialException:
        print("Unable to open the USB port.")
    SerialPortAvailable = True
except serial.SerialException:
	print('No USB port is available')
	port_serie = 0
	SerialPortAvailable = False

# Run the CMD loop (command line interpreter)
UVEX().cmdloop()

