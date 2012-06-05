'''Author: David Anderson'''
import tuio, pygame
#Creates the pygame screen
tela = pygame.display.set_mode((400, 400))
tela.fill((255, 255, 255))
class Listener(tuio.observer.AbstractListener):
    #Implements a Listener
    def notify(self, event):
        # Shows a square 5x5, on the appropriate coordinates
        tela.blit(pygame.Surface((5,5)),((int(400*event.object.xpos)), (int(400*event.object.ypos))))
        pygame.display.update()

listener = Listener("Event Listener", tuio.getEventManager())
tuio.mainLoop()
