import cv2
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


def resetAllData ():
    data = array([
                [(0,0),(0,0),(0,0)]
                ,[(0,0),(0,0),(0,0)]
                ,[(0,0),(0,0),(0,0)]
                ,[(0,0),(0,0),(0,0)]
                ], dtype=float32)
    return data


def resetObjData ():
    data = array([(0,0),(0,0),(0,0)], dtype=float32)
    return data


def displayMenu ():
    print ""
    print "------------------------------"
    print "'c' Change color of next point "
    print "'d' Toggle adding points."
    print "'h' Print this help menu."
    print "'l' Increase the light level threshold for the LED scanning"
    print "'n' Increase the number of robots to scan for"
    print "'p' Print the various point sets"
    print "'r' Remove the current color points."
    print "'s' Print Statuses"
    print "'t' Start tracking"
    print "'v' Toggle output view."
    print "'x' Reset all points."
    print "'Space bar' to pause/play video playback."
    print "'Esc' to exit."
    print "------------------------------"


def displayStatuses ():
    print ""
    print "------------------------------"
    print "Drawing:",drawing
    print "Color:",objectsColorName[objectIndex]
    print "Tracking:",tracking
    print "NumOfRobots:",numberOfRobots
    print "LED Level:",ledLevel
    print "------------------------------"


def displayPrint(a, b="", c=""):
    print "------------------------------"
    print a, b, c
    print "------------------------------"

def drawObject(objPts, colorCode):
    px,py = 0,0
    li = objPts.shape[0]-1
    sz = 2
    for x,y in objPts:    
        x,y = int(x),int(y)
        if x>0 or y>0:
            cv2.circle(outputImg, (x,y), sz, colorCode, -1, 8, 0)
            if px>0 or py>0:
                cv2.line(outputImg, (px,py), (x,y), colorCode, 1)
            elif (objPts[li][0]>0 or objPts[li][1]>0):
                px,py = objPts[li]
                cv2.line(outputImg, (px,py), (x,y), colorCode, 1)
                
            if sz>2:
                sz = 2
            px,py = x,y

def increaseLedLevel():
    global ledMin,ledLevel,ledMax
    if ledLevel == ledMax:
        ledLevel = ledMin
    else:
        ledLevel += 10
        if ledLevel > ledMax:
            ledLevel = ledMax

def increaseNumOfRobots():
    global numberOfRobots
    numberOfRobots += 1
    if numberOfRobots > 4:
        numberOfRobots = 1
        
        
            
def dist(p0, p1):
    return math.sqrt((p0[0] - p1[0])**2 + (p0[1] - p1[1])**2)


def findCenterOfBlob(Img, startPt, color):
    #print "startPt",startPt
    imgshape = Img.shape
    adjPt = startPt
    passes = 0
    while passes < 3:
        px,py = startPt
        while Img[py][px] == color:
            py += 1 #down
            if py >= imgshape[0]:
                break
        max_y = py-1

        px,py = startPt
        while  Img[py][px] == color:
            py -= 1 #up
            if py < 0:
                break
        min_y = py+1
        
        px,py = startPt
        while  Img[py][px] == color:
            px += 1 #right
            if px >= imgshape[1]:
                break
        max_x = px-1

        px,py = startPt
        while  Img[py][px] == color:
            px -= 1 #left
            if px < 0:
                break
        min_x = px+1

        adjPt[0] = min_x + (max_x-min_x)/2
        adjPt[1] = min_y + (max_y-min_y)/2
        
        if adjPt[0]==startPt[0] and adjPt[1]==startPt[1]:
            break
        startPt = adjPt
        passes+=1
    #print "adjPt",adjPt
    return adjPt


def findBots(Img):
    global objectsPts, numberOfRobots
    pts = cv2.goodFeaturesToTrack(Img, (numberOfRobots*3), 0.01, 12)
    if not pts:
        return []
    
    pts_shape = pts.shape
    pts = pts.reshape(pts_shape[0],pts_shape[2])
    for pnt in pts:
        pnt = findCenterOfBlob(Img, pnt, 255)
    print "------------------------------"
    allPts = objectsPts.reshape(12,2)
    for a_i, pnt_a in enumerate(pts):
        for pnt_b in pts:
            d = dist(pnt_a, pnt_b)
            if 17 <= d < 23: #it could a the rear short side of the triangle
                print pnt_a, pnt_b, d
                found = False
                for p in allPts:
                    if p[0]==pnt_a[0] and p[1]==pnt_a[1]:
                        found = True
                if not found:
                    p = pnt_a
                    
            elif 23 <= d <= 26: #it could be one of the longer sides of the triangle
                print pnt_a, pnt_b, d
                
    pts = pts.reshape(pts_shape)
    return pts




            
###############
## SETUP
###############
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
cv2.namedWindow("ArenaScanner")
key = -1

FPS = 30 # For video file playback only. 30 seems to match actual robot speed.
paused = False
drawing = True
tracking = False
drawObjects = True

dataview = 1

objectsColorCode = ((255,0,0), (0,240,0), (0,0,255), (0,210,210))
objectsColorName = ("Blue", "Green", "Red", "Yellow")
objectIndex = 0    #index of next object to add to
objectPntIndex = 0    #index of next point to add
objectsPts = resetAllData()
objectsNextPts = resetAllData()

numberOfRobots = 1
ledMin = 120
ledLevel = 120
ledMax = 255

pointer = (0,0)
gftt = resetAllData()

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
    nextImg = cv2.dilate(nextImg, None, iterations=1)
    retval,nextImg = cv2.threshold(nextImg, ledLevel, 255, cv2.THRESH_BINARY)
    if dataview == 1:
        outputImg = cv2.cvtColor(nextImg, cv2.COLOR_GRAY2BGR)
        

   
    #Point Tracking and Detection
    if tracking:
        prevPts = objectsPts.reshape((12,2))
        nextPts = zeros((12,2),dtype=float32)
        nextPts,status,err = cv2.calcOpticalFlowPyrLK(prevImg, nextImg, prevPts, nextPts)
        objectsPts = nextPts.reshape((4,3,2))
            
            
    #Draw points on output
    if drawObjects:
        objIndex = 0
        for objPts in objectsPts:
            drawObject(objPts, objectsColorCode[objIndex])
            objIndex+=1
        
        for objPts in gftt:
            drawObject(objPts, (255,255,0))
        
    if drawing:
        cv2.circle(outputImg, pointer, 2, objectsColorCode[objectIndex], -1, 8, 0)

        
    #Draw output        
    cv2.imshow("ArenaScanner", outputImg)

    
    #process key presses        
    key = cv2.waitKey(1)        
    if key == 27:  #esc
        break #exit

    if key == 32: #spacebar
        paused = not paused
        
    elif key == 99: #c key
        objectIndex += 1
        if objectIndex >= len(objectsColorCode):
            objectIndex = 0
        objectPntIndex = 0
        displayPrint("Color:",objectsColorName[objectIndex])

    elif key == 100: #d key
        drawing = not drawing
        displayStatuses()

    elif key == 102: #f key
        gftt = findBots(nextImg)
        
    elif key == 104: #h key
        displayMenu()

    elif key == 108: #l key
        increaseLedLevel()
        displayStatuses()
        
    elif key == 110: #n key
        increaseNumOfRobots()
        displayStatuses()
        
    elif key == 112: #p key
        print "------------------------------"
        print "objectsPts:",objectsPts.shape
        print objectsPts,"\n"
        print "gftt:",gftt.shape
        print gftt
        print "------------------------------"
        
    elif key == 114: #r key
        objectsPts[objectIndex] = resetObjData()
        objectPntIndex = 0
        displayPrint("Reset:",objectsColorName[objectIndex])
    
    elif key == 115: #s key
        displayStatuses()

    elif key == 116: #t key
        tracking = not tracking
        displayStatuses()

    elif key == 118: #v key
        dataview += 1
        if dataview > 1:
            dataview = 0
        displayPrint("Data View:",dataview)

    elif key == 120: #x key
        objectsPts = resetAllData()
        objectPntIndex = 0
        displayPrint("Reset All")
    
    elif key > 0:
        displayPrint("unknown key",key)

    prevImg = nextImg.copy()
print "Exiting."    

cap.release()
cv2.destroyAllWindows()
