#!/usr/bin/python
import sys, serial, time
import tuio

tracking = tuio.Tracking()

while 1:
    tracking.update()
    for obj in tracking.objects():
        print obj
        time.sleep(0.01)
    time.sleep(0.1)
