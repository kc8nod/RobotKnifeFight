#!/usr/bin/python
import sys, serial, time
import tuio

ARENA_SIZE_INCHES = (72.0, 48.0)
ARENA_SIZE_UNITS = (ARENA_SIZE_INCHES[0]*2.0, ARENA_SIZE_INCHES[1]*2.0)

NUM_HEADINGS = 16.0
DEG_PER_HEADING = 360.0 / NUM_HEADINGS

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
            session_id = obj.sessionid
            # TODO: Need to somehow match this to robot id

            x = int(obj.xpos * ARENA_SIZE_UNITS[0])
            # y-axis is inverted, to match Cartesian coordinates
            y = int((1.0 - obj.ypos) * ARENA_SIZE_UNITS[1])

            heading = int((obj.angle + DEG_PER_HEADING/2.0) / DEG_PER_HEADING)
            if heading >= 16:
                heading -= 16

            pos = 'pos ' + str(session_id) + ' ' + str(x) + ' ' + str(y) + ' ' + str(heading) + ' 1'

            print pos
            port.write(pos + '\n')
            time.sleep(0.01)
        time.sleep(0.1)

except KeyboardInterrupt:
    port.write('reset\n')
    tracking.stop()

