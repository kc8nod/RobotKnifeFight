'''Author: David Anderson'''
import tuio

class Listener(tuio.observer.AbstractListener):
    #Implements a Listener
    def notify(self, event):
        print self.name + str(event)
  
if __name__ == "__main__":
    listener = Listener("Event Listener", tuio.getEventManager())
    tuio.mainLoop()
