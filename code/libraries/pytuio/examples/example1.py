import tuio
tracking = tuio.Tracking()
print "loaded profiles:", tracking.profiles.keys()
print "list functions to access tracked objects:", tracking.get_helpers()

try:
    while 1:
        tracking.update()
        for obj in tracking.objects():
            print obj
except KeyboardInterrupt:
    tracking.stop()
