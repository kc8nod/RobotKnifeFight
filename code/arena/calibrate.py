#!/usr/bin/python
import sys, serial, time
import cmath
import math
import tuio



if __name__ == '__main__':
    tracking = tuio.Tracking()

    try:
        while 1:
            tracking.update()
            for obj in tracking.objects():
                id = obj.id

                
                #print obj.label
                print "id: %2d x: %.3f y: %.2f a: %.2f" % (obj.id, obj.xpos, obj.ypos, obj.angle)
                #print "    %s" % dir(obj)
                
                # time.sleep(0.01)
            # time.sleep(1)

    except KeyboardInterrupt:
        tracking.stop()

