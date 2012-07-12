#!/usr/bin/python
import sys, serial, time
import tuio
import arena

arena.ArenaPosition.read_config()

tracking = tuio.Tracking()

try:
    while 1:
        tracking.update()
        for obj in tracking.objects():
            
            camera_pos = arena.CameraPosition(obj)
            arena_pos = arena.ArenaPosition()
            
            arena_pos.set_camera(camera_pos)

            print "arena: x=%4d y=%4d a=%3d camera: x=%4d y=%4d a=%3d" % (arena_pos.xpos, arena_pos.ypos, arena_pos.angle,
                                                                          camera_pos)
            
            time.sleep(0.01)
        time.sleep(0.1)

except KeyboardInterrupt:
    tracking.stop()

