import tuio
tracking = tuio.Tracking()
listener = tuio.observer.Listener("Event Listener", tracking.eventManager)

try:
    while 1:
        tracking.update()
except KeyboardInterrupt:
    tracking.stop()
