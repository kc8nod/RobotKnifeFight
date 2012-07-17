#!/usr/bin/python
import sys, serial, time
import cmath
import tuio
import arena

arena.read_config()

if len(sys.argv) < 3:
    print 'Usage:', sys.argv[0], 'PORT', 'BAUDRATE'
    exit()

port_name = sys.argv[1]
baud_str = sys.argv[2]

port = serial.Serial(port_name, int(baud_str))
time.sleep(3)
port.write("start\n")

tracking = tuio.Tracking()

try:
    while 1:
        tracking.update()
        for obj in tracking.objects():
            
            camera_pos = arena.CameraPosition(obj)
            arena_pos = arena.ArenaPosition()
            
            arena_pos.set_camera(camera_pos)

            cmd_str = arena_pos.cmd_str()

            print cmd_str
            port.write(cmd_str + '\n')
            time.sleep(0.01)
        time.sleep(0.1)

except KeyboardInterrupt:
    port.write('reset\n')
    tracking.stop()

