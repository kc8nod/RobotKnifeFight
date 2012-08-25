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
            tracking.update()
            for obj in tracking.objects():
                obj.update()
                print obj.id,obj.xpos,obj.ypos
            
            

            
    except KeyboardInterrupt:
        tracking.stop()
        print "\nSTOP"
