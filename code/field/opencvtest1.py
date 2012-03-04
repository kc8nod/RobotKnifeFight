import cv

cv.NamedWindow("ArenaScannerOrg", 1)
#cv.NamedWindow("ArenaScanner", 2)
capture = cv.CaptureFromCAM(0)
while True:
    img = cv.QueryFrame(capture)
    image_size = cv.GetSize(img)
    
##    grayscale = cv.CreateImage(image_size, 8, 1)
##    cv.CvtColor(img, grayscale, cv.CV_RGB2GRAY)

    hc = cv.Load("C:\scratch\opencv\data\haarcascades\haarcascade_frontalface_default.xml")
    faces = cv.HaarDetectObjects(img, hc, cv.CreateMemStorage())
    for (x,y,w,h),n in faces:
        cv.Rectangle(img, (x,y), (x+w,y+h), 255)


    # imghsv = cv.CreateImage(image_size, 8, 3)
    # cv.CvtColor(img, imghsv, cv.CV_BGR2HSV)
    # imgThreshed = cv.CreateImage(image_size, 8, 1)
    # cv.InRangeS(imghsv, cv.Scalar(20, 100, 100), cv.Scalar(30, 255, 255), imgThreshed) ##yellow
    
    
    cv.ShowImage("ArenaScannerOrg", img)
    #cv.ShowImage("ArenaScanner", imgThre)
    if cv.WaitKey(10) == 27:  #esc key
        break
cv.DestroyWindow("ArenaScannerOrg")
#cv.DestroyWindow("ArenaScanner")
