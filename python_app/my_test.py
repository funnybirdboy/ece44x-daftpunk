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


devices = SerialPortDevice.enumerate_devices()
for device in devices:
	print device
	print dir(device)
	print device.name
	#print device.open()
	#print "fftsize:", device.get_fftsize()
	device.open()
	print device.get_name()
	print "fftsize:", device.get_fftsize()
	print "samplerate:", device.get_samplerate()
	print "audioport:", device.get_audioport()
#print devices
sys.exit()
