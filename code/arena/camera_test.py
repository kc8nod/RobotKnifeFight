#!/usr/bin/python
import sys, serial, time
import tuio
import arena

arena.read_config()

tracking = tuio.Tracking()

try:
    while True:
    
        while tracking.update():
            pass
            
        for obj in tracking.objects():
            
            camera_pos = arena.CameraPosition()
            camera_pos.set_tuio(obj)
            arena_pos = arena.ArenaPosition()
            
            arena_pos.set_camera(camera_pos)

            print "%2d: %s  ->  %s" % (camera_pos.id, str(camera_pos), str(arena_pos))
            
            
        time.sleep(1.0)
        print

except KeyboardInterrupt:
    tracking.stop()

