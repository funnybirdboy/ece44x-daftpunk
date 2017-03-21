# Spectrogram.py
# Copyright 2013 Tony DiCola (tony@tonydicola.com)

# Program to display a real-time spectrogram of data read from a device on the serial port.
# Depends on the following python libraries:
# - PySides (and Qt4)
# - Matplotlib
# - Numpy
# - PySerial
# For PySides, Matplotlib, and Numpy it is _highly_ recommended to install a prebuilt
# python distribution for scientific computing such as Anaconda or Enthought Canopy!

import sys
import SerialPortDevice
import time


devices = SerialPortDevice.enumerate_devices()
for device in devices:
	device.open()

	print "\n#################################################"
	print "Checking Variables"
	print "#################################################\n"
	print "name:", device.get_name()
	print "fftsize:", device.get_fftsize()
	print "samplerate:", device.get_samplerate()
	print "audioport:", device.get_audioport()
	print "mode:", device.get_mode()

	print "\n#################################################"
	print "Checking Modes"
	print "#################################################\n"
	for i in range(0, 3):
		print "Mode:", i
		device.set_mode(i)
		print "waiting..."
		for j in range(0, 5):
			#print j
			time.sleep(1)
	
	print "\n#################################################"
	print "Checking Brightness"
	print "#################################################\n"
	device.set_mode(3)
	for i in range(0,16):
		print "	", i
	   	device.set_brightness(i) 
		time.sleep(0.5)

	device.set_mode(0)
sys.exit()
