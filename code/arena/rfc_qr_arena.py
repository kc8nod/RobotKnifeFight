import cv, cv2
CV_CAP_PROP_FRAME_WIDTH = 3
CV_CAP_PROP_FRAME_HEIGHT = 4
import serial
import sys, math, time
from numpy import *
import re
import zbar
import Image

###############
## FUNCTIONS
###############

def displayMenu():
    print "------------------------------"
    print "'h' Print this help menu."
    print "'d' Toggle display mode."
    print "'t' Toggle verbose transmitting."
    print "'r' reset arena statuses."
    print "0-3 toggle bot alive."
    print "'v' Toggle verbose scanning."
    print "'Esc' to exit."
    print "------------------------------"

def displayPrint(a, b="", c=""):
    print "------------------------------"
    print a, b, c
    
def dist(p0, p1):
    return math.sqrt((p0[0] - p1[0])**2 + (p0[1] - p1[1])**2)

def findCenter(pts):
    x = 0
    y = 0
    for i in range(0,4):
        x += pts[i][0]
        y += pts[i][1]
    return (int(x/4), int(y/4))

def findDiffPt(pt0, pt1):
    x = int((pt0[0]+pt1[0])/2)
    y = int((pt0[1]+pt1[1])/2)
    return (x,y)

def findDiffs(pt0, pt1):
    x = pt1[0]-pt0[0]
    y = pt1[1]-pt0[1]
    return (x,y)

def drawBorder(img, symbol, color, thickness):
    cv2.line(img, symbol[0], symbol[1], color, thickness)
    cv2.line(img, symbol[1], symbol[2], color, thickness)
    cv2.line(img, symbol[2], symbol[3], color, thickness)
    cv2.line(img, symbol[3], symbol[0], color, thickness)
            

###############
## SETUP
###############

index_pattern = re.compile('\d+')
bot_pattern = re.compile('http://(\d{3})')
arena_pattern = re.compile('A(\d{3})\-C(\d)')
dead_pattern = re.compile('DEAD(\d{3})')

cameraindex = 0
resolutionindex = 0
resolutions = [(640,480),(1280,720),(1920,1080)]

cv2.namedWindow("ArenaScanner")

cv2.startWindowThread()

key = -1

displayMode = 1

verboseScan = False
verboseTransmit = False

xmitportindex = 0

#Prompt for settings
#Camera ID
displayPrint("Enter the camera's index")
in_str = raw_input("defaults [%1d] " % cameraindex)
match = index_pattern.match(in_str)
if match:
    print "using "+match.group(0)
    cameraindex = int(match.group(0))
else:
    print "using default"

#Resolution
print("Enter the camera's resolution ")
for idx,(x,y) in enumerate(resolutions):
    print "[{}] {}x{}".format(idx,x,y)
in_str = raw_input("defaults [%1d] " % resolutionindex)
match = index_pattern.match(in_str)
if match:
    print "using "+match.group(0)
    resolutionindex = int(match.group(0))
else:
    print "using default"

#RKF_Xmit serial port
displayPrint("Enter the RKF_Xmit serial port's index /dev/ttyUSB[?]")   
in_str = raw_input("defaults [%1d] " % xmitportindex)
match = index_pattern.match(in_str)
if match:
    print "using "+match.group(0)
    xmitportindex = int(match.group(0))
else:
    print "using default"
xmit_port = serial.Serial("/dev/ttyUSB"+str(xmitportindex), 115200, timeout=1)

cap = cv2.VideoCapture(cameraindex)
cap.set(CV_CAP_PROP_FRAME_WIDTH, resolutions[resolutionindex][0])
cap.set(CV_CAP_PROP_FRAME_HEIGHT, resolutions[resolutionindex][1])
  
scanner = zbar.ImageScanner()
scanner.set_config(0, zbar.Config.ENABLE, 0) #disable all symbols
scanner.set_config(zbar.Symbol.QRCODE, zbar.Config.ENABLE, 1) #enable QR codes

colorCode = ((255,0,0), (0,240,0), (0,0,255), (29,227,245), (224,27,217)) #Blue, Green, Red, Yellow, Purple

displayMenu()

arenaId = 0
arenaSize = (70.5, 46.5) #Arena size in inches

arenaCorners = [(0,0),(0,0),(0,0),(0,0)]
arenaInnerCorners = [(0,0),(0,0),(0,0),(0,0)]

botLocAbs = [(0,0), (0,0), (0,0), (0,0)]
botLocArena = [(0,0), (0,0), (0,0), (0,0)]
botHeading = [0, 0, 0, 0]
botAlive = [True, True, True, True]

gameOn = False

###############
## LOOP
###############
while True:
    #get next frame from capture device
    success, origImg = cap.read()
    if not success:
        displayPrint("Error reading from camera.");
        break;

    #Apply image transformations
    modImg = cv2.cvtColor(origImg, cv2.COLOR_BGR2GRAY) #convert to grayscale
    width = size(modImg, 1)
    height = size(modImg, 0)
    if displayMode >= 2:
        outputImg = zeros((height,width,3), uint8) #create a blank image
    else:
        outputImg = cv2.cvtColor(modImg, cv2.COLOR_GRAY2BGR)
    #Scan for QR codes
    raw = modImg.tostring()
    image = zbar.Image(width, height, 'Y800', raw)
    scanner.scan(image)
    #draw borders on detected symbols and record object locations
    for symbol in image:
        if verboseScan:
		    print symbol.data, symbol.location

        #Arena Corners
        match = arena_pattern.match(symbol.data)
        if match:
            a = int(match.group(1))
            c = int(match.group(2))
            if displayMode < 3:
                drawBorder(outputImg, symbol.location, colorCode[0], 2)
                pt = (symbol.location[1][0]+8, symbol.location[1][1]-25)  
                cv2.putText(outputImg, str(a)+'-'+str(c), pt, cv2.FONT_HERSHEY_PLAIN, 1.5, colorCode[0], 2)
            if a == arenaId: #found a corner for the arena
                arenaCorners[c] = symbol.location[c]
                ic = (c+2)%4
                arenaInnerCorners[c] = symbol.location[ic]

        #Bot Symbol
        match = bot_pattern.match(symbol.data)
        if match:
            pt = findCenter(symbol.location)
            botId = int(match.group(1))
            
            #update the bots location
            botLocAbs[botId] = pt
            wallCenterX = findDiffPt(arenaCorners[0],arenaCorners[3])
            wallCenterY = findDiffPt(arenaCorners[0],arenaCorners[1])
            maxX = arenaCorners[3][0]-arenaCorners[0][0]
            maxY = arenaCorners[1][1]-arenaCorners[0][1]
            arenaPtX = pt[0]-wallCenterY[0]
            arenaPtY = pt[1]-wallCenterX[1]
            if maxX>0 and maxY>0:
                arenaPtX = int(float(arenaPtX)/float(maxX)*arenaSize[0])
                arenaPtY = int(float(arenaPtY)/float(maxY)*arenaSize[1])
            botLocArena[botId] = (arenaPtX, arenaPtY)

            #update the bots heading
            x = symbol.location[3][0] - symbol.location[0][0]
            y = symbol.location[3][1] - symbol.location[0][1]
            h = math.degrees((math.pi/2) - math.atan2(y,x))
            h = int(math.floor((h+11.25)/22.5))
            if h < 0: h = 16 + h
            botHeading[botId] = h

            #draw the borders, heading, and text for bot symbol
            if displayMode < 3:
                drawBorder(outputImg, symbol.location, colorCode[1], 2)                  
                pt = (pt[0]-20, pt[1]+10)            
                cv2.putText(outputImg, match.group(1), pt, cv2.FONT_HERSHEY_PLAIN, 1.5, colorCode[1], 2)
                ptdiff = findDiffs(symbol.location[1], symbol.location[0])
                pt0 = findDiffPt(symbol.location[0], symbol.location[3])
                pt1 = (pt0[0]+int(ptdiff[0]*1.7), pt0[1]+int(ptdiff[1]*1.7))
                cv2.line(outputImg, pt0, pt1, colorCode[1], 2)

            #build xmit string
            xmitcom = "pos %d %d %d %d %d\n" % (botId,botLocArena[botId][0],botLocArena[botId][1],botHeading[botId],botAlive[botId])
            xmit_port.write(xmitcom)
            #print xmitcom
    
        #Bot Dead
        match = dead_pattern.match(symbol.data)
        if match:
            if displayMode < 3:
                drawBorder(outputImg, symbol.location, colorCode[2], 2)
                pt = findCenter(symbol.location)
                pt = (pt[0]-20, pt[1]+10)            
                cv2.putText(outputImg, match.group(1), pt, cv2.FONT_HERSHEY_PLAIN, 1.5, colorCode[2], 2)
            botid = int(match.group(1))            
            botAlive[botid] = False #mark bot as dead

    if verboseScan:
        print

    #Draw Objects
    #Arena
    drawBorder(outputImg, arenaCorners, colorCode[0], 2)  
    drawBorder(outputImg, arenaInnerCorners, colorCode[0], 1)  

    #Last Know Bot Locations
    for idx,pt in enumerate(botLocAbs):
        if pt[0] == 0 and pt[1] == 0:
            continue
        if botAlive[idx]:
            color = colorCode[3]
        else:
            color = colorCode[2]
        cv2.circle(outputImg, pt, 30, color, 2)
        textPt = (pt[0]-8, pt[1]+8)
        cv2.putText(outputImg, str(idx), textPt, cv2.FONT_HERSHEY_PLAIN, 1.5, color, 2)
        ang = botHeading[idx]*math.pi/8
        pt0 = ((pt[0]+int(math.cos(ang)*30)), (pt[1]-int(math.sin(ang)*30)))
        pt1 = ((pt[0]+int(math.cos(ang)*30*4)), (pt[1]-int(math.sin(ang)*30*4)))
        cv2.line(outputImg, pt0, pt1, color, 2)

    #Draw Statuses
    lh = 20 #line height
    pt = (0,40)
    for idx in range(0,4):
        if botLocAbs[idx][0]==0 and botLocAbs[idx][1]==0:
            continue
        status = str(idx)+":"+str(botLocArena[idx])+' '+str(botHeading[idx])
        cv2.putText(outputImg, status, pt, cv2.FONT_HERSHEY_PLAIN, 1.5, colorCode[4], 1)
        pt = (pt[0],pt[1]+lh)

    #Display Output       
    if displayMode == 0: #display source image
        cv2.putText(origImg, "source", (0,15), cv2.FONT_HERSHEY_PLAIN, 1.5, colorCode[4], 1)
        cv2.imshow("ArenaScanner", origImg)

    elif displayMode == 1: #display source with data overlay
        cv2.putText(outputImg, "scan", (0,15), cv2.FONT_HERSHEY_PLAIN, 1.5, colorCode[4], 1)
        cv2.imshow("ArenaScanner", outputImg)

    elif displayMode == 2: #display only data overlay
        cv2.putText(outputImg, "data", (0,15), cv2.FONT_HERSHEY_PLAIN, 1.5, colorCode[4], 1)
        cv2.imshow("ArenaScanner", outputImg)

    elif displayMode == 3: #display the only the bots point of view
        cv2.putText(outputImg, "bot", (0,15), cv2.FONT_HERSHEY_PLAIN, 1.5, colorCode[4], 1)
        cv2.imshow("ArenaScanner", outputImg)

    #Process key presses        
    key = cv2.waitKey(10)        
    if key>0:
        if key == 27:  #esc
            break #exit

        elif chr(key) == 'h': #h key
            displayMenu()
            
        elif chr(key) == 'v': #v key
            verboseScan = not verboseScan
            displayPrint("verboseScan:",verboseScan)

        elif chr(key) == 't': #t key
            verboseTransmit = not verboseTransmit
            displayPrint("verboseTransmit:",verboseTransmit)

        elif chr(key) == 'd': #d key
            displayMode += 1
            if displayMode > 3:
                displayMode = 0
            displayPrint("displayMode:",displayMode)
        
        elif chr(key) == 'r': #r key
            botAlive = [True, True, True, True]
            botLocAbs = [(0,0), (0,0), (0,0), (0,0)]
            botLocArena = [(0,0), (0,0), (0,0), (0,0)]
            botHeading = [0, 0, 0, 0]
            displayPrint("reset arena statuses")
            arenaCorners = [(0,0),(0,0),(0,0),(0,0)]
            arenaInnerCorners = [(0,0),(0,0),(0,0),(0,0)]
            xmit_port.write("reset\n")

        elif 48 <= key and key <=51: #0-3
            botid = key-48
            botAlive[botid] = not botAlive[botid]
            displayPrint("bot "+str(botid)+" alive:",botAlive[botid])
        
        elif chr(key) == ' ': #space key
            if gameOn:            
                xmit_port.write("stop\n")
            else:
                xmit_port.write("start\n")
            gameOn = not gameOn
            displayPrint("gameOn:",gameOn)

        else:
            displayPrint("unassigned key", key, chr(key))
    
    #print serial data
    if verboseTransmit and gameOn:
        print(xmit_port.readline().strip());
    xmit_port.flushInput()
        

###############
## END LOOP
###############
cap.release()
cv2.destroyAllWindows()
xmit_port.write('reset\n')
xmit_port.close()        
print
print "Exiting."
print    



