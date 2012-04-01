#!/usr/bin/python

import sys, math
import itertools
import cv

def detectFeatures(img):
  """Detect features.
  \param img Grayscale input image.
  \return Feature iterator.
  """
  eig_image = cv.CreateImage(cv.GetSize(img), cv.IPL_DEPTH_32F, 1)
  temp_image = cv.CreateImage(cv.GetSize(img), cv.IPL_DEPTH_32F, 1)
  return cv.GoodFeaturesToTrack(img, eig_image, temp_image, 300, 0.01, 10, None, 3, 0, 0.04)

def distance(p0, p1):
  return math.sqrt((p0[0] - p1[0])**2 + (p0[1] - p1[1])**2)
  
def groupPoints(pointList, minDistance):
  pointGroups = []
  for point in pointList:
    done = False  # done grouping this point?
    for pointGroup in pointGroups:
      for point2 in pointGroup:
        if done == False:
          if distance(point, point2) < minDistance:
            pointGroup.append(point)
            done = True
    if done == False:  # didn't find a good group
      newGroup = [point]
      pointGroups.append(newGroup)
  return pointGroups
  

if __name__ == '__main__':
  img = cv.LoadImage(sys.argv[-1])
  imgGrey = cv.LoadImage(sys.argv[-1], cv.CV_LOAD_IMAGE_GRAYSCALE)

  # smooth
  imgGreySm = cv.CreateImage(cv.GetSize(imgGrey), imgGrey.depth, 1)
  cv.Smooth(imgGrey, imgGreySm, smoothtype=cv.CV_MEDIAN, param1=9)

  # detect features
  features = detectFeatures(imgGreySm)
  featureList = list(features)
  for (x,y) in featureList:
    cv.Circle(img, (int(x),int(y)), 3, (0,255,0), -1, 8, 0)

  # detect objects
  minDistance = 50
  featureGroups = groupPoints(featureList, minDistance)
  for group in featureGroups:
    print group
    for i in range(len(group) - 1):
      cv.Line(img, (int(group[i][0]), int(group[i][1])), (int(group[i+1][0]), int(group[i+1][1])), (0,0,255))
  
  cv.NamedWindow('display')
  cv.ShowImage('display', img)
  cv.WaitKey(0)

