#!/usr/bin/python2
# -*- coding: utf-8 -*-
import serial
import dbus
import subprocess
import time
import os

def runner(cmd):
    PIPE = subprocess.PIPE
    p = subprocess.Popen(cmd, shell=True, stdin=PIPE, stdout=PIPE,
                         stderr=subprocess.STDOUT, close_fds=True)
    return p.stdout
# reset old connections
cmd = "sudo rfcomm unbind all"
runner(cmd)
# find my remote device
cmd = "hcitool name 00:01:95:0C:C4:A5"
if 'test serial' not in runner(cmd).readline():
    print 'Device not aviable. Exiting'
    #exit()
# connect to this
cmd = "sudo rfcomm bind all"
runner(cmd)
cmd = "sudo rfcomm show all"
if 'channel 1 clean' not in runner(cmd).read():
    print 'Rules not load. Exiting'
    exit()

ser = serial.Serial('/dev/rfcomm0', 115200, timeout=1)
bus = dbus.SessionBus()
am = bus.get_object('org.kde.amarok', '/Player')

commands = {
            'play': [am.PlayPause, []],
            'next': [am.Next, []],
            'prev': [am.Prev, []],
            'mute': [am.Mute, []],
            'vol+': [am.VolumeUp, [1,]],
            'vol-': [am.VolumeDown, [1,]]
            }

print 'Connected'

try:
    while 1:
        try:
            line = ser.read(3)
        except serial.serialutil.SerialException:
            ser.close()
            time.sleep(0.5)
            while 1:
                try:
                    ser = serial.Serial('/dev/rfcomm0', 115200, timeout=1)
                    break
                except serial.serialutil.SerialException:
                    time.sleep(2)
        if len(line) == 3:
            if line[0] == 'C':
                print 'Command'
                if ord(line[1]) == 0x00 and ord(line[2]) == 0x00:
                    print 'Device ping'
                    ser.write('A')
                    ser.write(chr(0x00))
                    ser.write(chr(0x02))
                    ser.write('OK')
                    print 'Ansver to device'
            else:
                line += ser.readline()
                cmd = line.strip()
                if cmd in commands:
                    current = commands[cmd]
                    current[0](*current[1])
except KeyboardInterrupt:
    ser.close()
    del am
print 'Exiting'
# cleaning
cmd = "sudo rfcomm unbind all"
runner(cmd)