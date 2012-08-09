#!/usr/bin/python
import sys, time
import cmath
import math
import tuio
import arena
import re

xy_pattern = re.compile('(\d+(\.\d*)?|\.\d+),(\d+(\.\d*)?|\.\d+)')

if __name__ == '__main__':
    tracking = tuio.Tracking()
  
    arena_corner1 = arena.ArenaPosition(0.0, 0.0, 0.0)
    arena_corner2 = arena.ArenaPosition(70.5, 46.5, 0.0)
    

    try:
        arena.read_config()
    except:
        print "no config file found. writing a new one"
  
    try:

        print "Put exactly one glyph in the camera's field of view."

        while True:
            tracking.update()
            objs = list(tracking.objects())
            if len(objs) == 1:
                break
                
                
        print "Move the glyph to the 0,0 point of the arena..."
        raw_input("then press return to continue.")
     
        tracking.update()
        camera_pos1 = arena.CameraPosition()
        obj = tracking.objects().next()
        print "debug: raw tuio object:", obj
        camera_pos1.set_tuio(obj)
        
        print "arena_corner1: %s" % str(arena_corner1)
        print "camera_pos1: %s" % str(camera_pos1)

        print
        print "now move the glyph to the opposite corner"
        raw_input("then press return to continue.")
         
        
        tracking.stop()
        tracking = tuio.Tracking()

        while True:
            tracking.update()
            objs = list(tracking.objects())
            if len(objs) == 1:
                break
        
        tracking.update()
        camera_pos2 = arena.CameraPosition()
        obj = tracking.objects().next()
        print "debug: raw tuio object:", obj
        camera_pos2.set_tuio(obj)
        
        print
        print "enter the x,y coordinates of the opposite corner"
        in_str = raw_input("or press return to accept the defaults [%.1f,%.1f] " % (arena_corner2.xpos, arena_corner2.ypos))
        match = xy_pattern.match(in_str)
        if match:
            arena_corner2.xpos = float(match.group(1))
            arena_corner2.ypos = float(match.group(3))
            
        else:
            print "using defaults"
        

        print
        print "enter the cameras max resolution"
        in_str = raw_input("or press return to accept the defaults [%.1f,%.1f] " % (camera_pos2.camera_x_max, camera_pos2.camera_y_max))
        match = xy_pattern.match(in_str)
        if match:
            camera_pos2.camera_x_max = float(match.group(1))
            camera_pos2.camera_y_max = float(match.group(3))
            
        else:
            print "using defaults"
 
        print "arena_corner2: %s" % str(arena_corner2)
        print "camera_pos2: %s" % str(camera_pos2)
        
        arena.ArenaPosition.calibrate(camera_pos1, arena_corner1,
                                      camera_pos2, arena_corner2)
        
        print "scale: %s" % arena.ArenaPosition.scale
        print "x_offset: %s" % arena.ArenaPosition.x_offset
        print "y_offset: %s" % arena.ArenaPosition.y_offset
        print "angle_offset: %s" % arena.ArenaPosition.angle_offset
        
        arena.write_config()
        
    finally:
        tracking.stop()

    print "done"
