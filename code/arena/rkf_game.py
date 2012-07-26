#!/usr/bin/python
import sys, serial, time
import cmath
import tuio
import arena
import datetime

arena.read_config()

if len(sys.argv) < 3:
    print 'Usage:', sys.argv[0], 'PORT', 'BAUDRATE'
    exit()

port_name = sys.argv[1]
baud_str = sys.argv[2]
port = serial.Serial(port_name, int(baud_str))
time.sleep(3)

for i in range(10):
    print port.readline()[:-1]

tracking = tuio.Tracking()

time.sleep(1)
print "Starting Game on",port_name,baud_str+"bps"
port.write("start\n")
time.sleep(.01)

try:
    while 1:
        tracking.update()

        str_now = str(datetime.datetime.now())[11:23]
        
        for obj in tracking.objects():
            camera_pos = arena.CameraPosition()
            camera_pos.set_tuio(obj)

            arena_pos = arena.ArenaPosition()
            arena_pos.set_camera(camera_pos)

            if arena_pos.id != -1:
                cmd_str = arena_pos.command_str()
                print str_now,cmd_str
                port.write(cmd_str + '\n')
                time.sleep(0.01)

        print str_now,port.readline()[:-1]
        time.sleep(0.1)

except KeyboardInterrupt:
    print "\n",str_now,"stop"
    port.write('stop\n')
    time.sleep(1)
    port.close()
    tracking.stop()
    
    

