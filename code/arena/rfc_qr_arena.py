import cv, cv2
import sys, math, time
from numpy import *

###############
## FUNCTIONS
###############

def onMouse (event, x, y, flags, param):
    #print event, x, y, flags, param
    global objectsPts
    global objectPntIndex
    global outputImg
    global pointer
    
    if event == cv2.EVENT_LBUTTONUP:
        #print "LBUTTONUP", x, y, flags, param
        if drawing:
            s = objectsPts.shape
            objectsPts.itemset((objectIndex,objectPntIndex,0), x)
            objectsPts.itemset((objectIndex,objectPntIndex,1), y)
            objectPntIndex += 1
            if objectPntIndex >= s[1]:
                objectPntIndex = 0
            print "Added Point:",objectIndex,objectPntIndex,x,y
            
    if event == cv2.EVENT_MOUSEMOVE:
        pointer = (x, y)

def displayMenu ():
    print ""
    print "------------------------------"
    print "'h' Print this help menu."
    print "'n' Increase the number of robots to scan for"
    print "'f' 'g' Changes focus"
    print "'v' Toggle output view."
    print "'s' Display statuses."
    print "'Space bar' to pause/play video playback."
    print "'Esc' to exit."
    print "------------------------------"


def displayStatuses ():
    print ""
    print "------------------------------"
    print "NumOfRobots:",numberOfRobots
    print "Focus:",focus
    print "------------------------------"


def displayPrint(a, b="", c=""):
    print "------------------------------"
    print a, b, c
    print "------------------------------"


def increaseNumOfRobots():
    global numberOfRobots
    numberOfRobots += 1
    if numberOfRobots > 4:
        numberOfRobots = 1

def increaseFocus():
    global focus, cap
    focus += 1
    if focus > 40:
        focus = 1
    cap.set(CV_CAP_PROP_FOCUS, focus)

def decreaseFocus():
    global focus, cap
    focus -= 1
    if focus < 1:
        focus = 40
    cap.set(CV_CAP_PROP_FOCUS, focus)
    
def dist(p0, p1):
    return math.sqrt((p0[0] - p1[0])**2 + (p0[1] - p1[1])**2)



            
###############
## SETUP
###############
CV_CAP_PROP_FRAME_WIDTH = 3
CV_CAP_PROP_FRAME_HEIGHT = 4
CV_CAP_PROP_FOCUS = 28

focus = 1

playFromFile = False
loopVideo = False            
if len(sys.argv) > 1:
  playFromFile = True
  cap = cv2.VideoCapture(sys.argv[1])
  if len(sys.argv) > 2:
    if sys.argv[2] == "-l":
      loopVideo = True
else:
  cap = cv2.VideoCapture(0)
  cap.set(CV_CAP_PROP_FRAME_WIDTH, 1920)
  cap.set(CV_CAP_PROP_FRAME_HEIGHT, 1080)
  cap.set(CV_CAP_PROP_FOCUS, focus)
cv2.namedWindow("ArenaScanner")
key = -1

FPS = 30 # For video file playback only. 30 seems to match actual robot speed.
paused = False
drawing = False 
dataview = 1

numberOfRobots = 1

pointer = (0,0)

cv2.setMouseCallback("ArenaScanner", onMouse)
success, prevImg = cap.read()

displayMenu()
displayStatuses()

prevTime = time.time()

###############
## LOOP
###############
while True:
    if playFromFile == True and paused == False:
        while time.time() < prevTime + 1.0/FPS:
            time.sleep(0.01)
        prevTime = time.time()

    if playFromFile == False or paused == False:
        #get next frame from capture device
        success, nextImg = cap.read()
        lastImg = nextImg
        if not success:
            if loopVideo:
                cap = cv2.VideoCapture(sys.argv[1])
                continue
            break;
    else:
        nextImg = lastImg


    #Apply image transformations
    if dataview == 0:
        outputImg = nextImg.copy()
    nextImg = cv2.cvtColor(nextImg, cv2.COLOR_BGR2GRAY)
    #nextImg = cv2.dilate(nextImg, None, iterations=1)
    #retval,nextImg = cv2.threshold(nextImg, 100, 255, cv2.THRESH_BINARY)
    if dataview == 1:
        outputImg = cv2.cvtColor(nextImg, cv2.COLOR_GRAY2BGR)
        
        
    #Draw output        
    cv2.imshow("ArenaScanner", outputImg)

    
    #process key presses        
    key = cv2.waitKey(1)        
    if key == 27:  #esc
        break #exit

    if key == 32: #spacebar
        paused = not paused
      
    elif key == 104: #h key
        displayMenu()
        
    elif key == 110: #n key
        increaseNumOfRobots()
        displayStatuses()

    elif key == 115: #s key
        displayStatuses()

    elif key == 102: #f key
        decreaseFocus()
        displayStatuses()

    elif key == 103: #g key
        increaseFocus()
        displayStatuses()
    
    elif key == 118: #v key
        dataview += 1
        if dataview > 1:
            dataview = 0
        displayPrint("Data View:",dataview)

    elif key > 0:
        displayPrint("unknown key",key)

    prevImg = nextImg.copy()
print "Exiting."    

cap.release()
cv2.destroyAllWindows()
