#!/usr/bin/python

import sys
import cv

def detectFeatures(img):
  """Detect features.
  \param img Grayscale input image.
  \return Feature iterator.
  """
  eig_image = cv.CreateImage(cv.GetSize(img), cv.IPL_DEPTH_32F, 1)
  temp_image = cv.CreateImage(cv.GetSize(img), cv.IPL_DEPTH_32F, 1)
  return cv.GoodFeaturesToTrack(img, eig_image, temp_image, 300, 0.01, 10, None, 3, 0, 0.04)

if __name__ == '__main__':
  img = cv.LoadImage(sys.argv[1])
  imgGrey = cv.LoadImage(sys.argv[1], cv.CV_LOAD_IMAGE_GRAYSCALE)
  imgGreySm = cv.CreateImage(cv.GetSize(imgGrey), imgGrey.depth, 1)
  cv.Smooth(imgGrey, imgGreySm, smoothtype=cv.CV_MEDIAN, param1=9)
  features = detectFeatures(imgGreySm)
  for (x,y) in features:
    cv.Circle(img, (int(x),int(y)), 3, (0,255,0), -1, 8, 0)
  cv.NamedWindow('display')
  cv.ShowImage('display', img)
  cv.WaitKey(0)

