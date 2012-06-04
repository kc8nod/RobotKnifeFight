class CursorEvent(object):
    def __init__(self, objeto, message):
        self.object = objeto.objects[message[3]]
        self.message = message
    def __str__(self): 
        return str(self.object)
         
class ObjectEvent(object):
    def __init__(self, objeto, message):
        self.object = objeto.objects[message[4]]
        self.message = message
    def __str__(self): 
        return str(self.object) 
            
class AbstractEventManager:
    def register(self, listener):
        raise NotImplementedError("Must subclass me")
 
    def unregister(self, listener):
        raise NotImplementedError("Must subclass me")
 
    def notify_listeners(self, event):
        raise NotImplementedError("Must subclass me")
 
class AbstractListener:
    def __init__(self, name, subject):
        self.name = name
        subject.register(self)
    def notify(self, event):
        raise NotImplementedError("Must subclass me")
 
class EventManager(AbstractEventManager):
    def __init__(self):
        self.listeners = []
        self.data = None

    # Implement abstract Class AbstractEvent
    def register(self, listener):
        self.listeners.append(listener)
 
    def unregister(self, listener):
        self.listeners.remove(listener)
 
    def notify_listeners(self, event):
        for listener in self.listeners:
            listener.notify(event)


