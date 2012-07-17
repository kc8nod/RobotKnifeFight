#!/usr/bin/python
import sys, serial, time
import tuio
import arena

arena.read_config()

tracking = tuio.Tracking()

try:
    while 1:
        tracking.update()
        for obj in tracking.objects():
            
            camera_pos = arena.CameraPosition(obj)
            arena_pos = arena.ArenaPosition()
            
            arena_pos.set_camera(camera_pos)

            print "%2d: %s  ->  %s" % (camera_pos.id, str(camera_pos), str(arena_pos))
            
            time.sleep(0.01)
        time.sleep(0.1)

except KeyboardInterrupt:
    tracking.stop()

