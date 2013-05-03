import cv, cv2
import sys, math, time
from numpy import *
import re
import zbar
import Image

###############
## FUNCTIONS
###############

def displayMenu ():
    print ""
    print "------------------------------"
    print "'h' Print this help menu."
    print "'f' 'g' Changes focus"
    print "'v' Toggle output view."
    print "'s' Display statuses."
    print "'Space bar' to pause/play video playback."
    print "'Esc' to exit."

def displayStatuses ():
    print ""
    print "------------------------------"
    print "Focus:",focus

def displayPrint(a, b="", c=""):
    print "------------------------------"
    print a, b, c

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

index_pattern = re.compile('\d+')
cameraindex = 0
focus = 2

playFromFile = False
loopVideo = False            
if len(sys.argv) > 1:
    playFromFile = True
    cap = cv2.VideoCapture(sys.argv[1])
    if len(sys.argv) > 2:
        if sys.argv[2] == "-l":
            loopVideo = True
else:
    print
    print "Enter the camera's index"
    in_str = raw_input("defaults [%1d] " % cameraindex)
    match = index_pattern.match(in_str)
    if match:
        print match.group(0)
        cameraindex = int(match.group(0))
    else:
        print "using defaults"
        
    cap = cv2.VideoCapture(cameraindex)
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 1280)
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 720)
    cap.set(CV_CAP_PROP_FOCUS, focus)
  
cv2.namedWindow("ArenaScanner")

cv2.startWindowThread()

key = -1

FPS = 30 # For video file playback only. 30 seems to match actual robot speed.
paused = False

dataview = 1

success, prevImg = cap.read()

displayMenu()
displayStatuses()

prevTime = time.time()

scanner = zbar.ImageScanner()
#scanner.parse_config('enable')
scanner.set_config(0, zbar.Config.ENABLE, 0) #disable all symbols
scanner.set_config(zbar.Symbol.QRCODE, zbar.Config.ENABLE, 1) #enable QR codes

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
        prevImg = nextImg
        if not success:
            if loopVideo:
                cap = cv2.VideoCapture(sys.argv[1])
                continue
            break;
    else:
        nextImg = prevImglastImg


    #Apply image transformations
    if dataview == 0:
        outputImg = nextImg.copy()
    nextImg = cv2.cvtColor(nextImg, cv2.COLOR_BGR2GRAY)
    if dataview == 1:
        outputImg = cv2.cvtColor(nextImg, cv2.COLOR_GRAY2BGR)
        
    #Scan for QR codes
    width = size(nextImg, 1)
    height = size(nextImg, 0)
    #print width, height
    raw = nextImg.tostring()
    image = zbar.Image(width, height, 'Y800', raw)
    scanner.scan(image)
    for symbol in image:
        # do something useful with results
        print symbol.type, '"%s"' % symbol.data, ' location',symbol.location



    #Draw output        
    cv2.imshow("ArenaScanner", outputImg)

    
    #process key presses        
    key = cv2.waitKey(10)        
    if key == 27 or key==1048603:  #esc
        break #exit

    elif key == 32: #spacebar
        paused = not paused
      
    elif key == 104: #h key
        displayMenu()
        
    elif key == 115: #s key
        displayStatuses()

    elif key == 102: #f key
        decreaseFocus()
        displayStatuses()

    elif key == 103 or key==1048679: #g key
        increaseFocus()
        displayStatuses()
    
    elif key == 118 or key==1048694: #v key
        dataview += 1
        if dataview > 1:
            dataview = 0
        displayPrint("Data View:",dataview)

    elif key > 0:
        displayPrint("unknown key",key)

    prevImg = nextImg.copy()

cap.release()
cv2.destroyAllWindows()        
print "Exiting."    



