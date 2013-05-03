#!/usr/bin/python
import sys, time
import cmath
import math
import tuio
import arena
import re

if __name__ == '__main__':
    tracking = tuio.Tracking()

    tracking.start()

    print "loaded profiles:", tracking.profiles.keys()
    print "list functions to access tracked objects:", tracking.get_helpers()


    try:
        while True:
            while tracking.update():
                objs = tracking.objects().next()
                print objs
                
            
    except KeyboardInterrupt:
        while tracking.update():
            objs = tracking.objects().next()
            print objs
        tracking.stop()
        print "\nSTOP"
