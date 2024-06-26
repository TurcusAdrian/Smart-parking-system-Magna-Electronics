import cv2
import imutils
import numpy as np
import pytesseract
from libcamera import controls
import time

from PIL import Image
from picamera2 import Picamera2, Preview

from license_plate_updated import RoLicensePlate 

camera = Picamera2()
camera_config = camera.create_preview_configuration()
camera.configure(camera_config)

#name = "img"
#for i in range(5):
  #  name = name + i + ".jpg"
  #  camera.start_preview(Preview.QTGL)
  #  camera.start()
  #  time.sleep(15)
  #  camera.capture_file(name)
   # camera.stop_preview()
   ## img = cv2.imread(name)
   # img = cv2.resize(img, (1920, 640))

#best_img =
sharp_ker = np.array([[0,-1,0],[-1,5,-1],[0,-1,0]])
camera.start_preview(Preview.QTGL)
camera.start()
time.sleep(10)
camera.capture_file("alabala.jpg")
camera.stop_preview()
img = cv2.imread("alabala.jpg")
img = cv2.resize(img, (1920, 640))
img = cv2.filter2D(img,-1,sharp_ker)
cv2.imshow('poza_originala',img)



cv2.waitKey()
img = cv2.resize(img, (620,480) )
gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY) #convert to grey scale

cv2.imshow('poza_originala',gray)
cv2.waitKey()

gray = cv2.bilateralFilter(gray,11,17,17)
cv2.imshow('poza_originala',gray)
cv2.waitKey()

edged = cv2.Canny(gray,30,200)
cv2.imshow('poza_originala',edged)
cv2.waitKey()

cnts = cv2.findContours(edged.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
cnts = imutils.grab_contours(cnts)
cnts = sorted(cnts, key = cv2.contourArea, reverse = True)[:10]
screenCnt = None

for c in cnts:

 # approximate the contour

 peri = cv2.arcLength(c, True)

 approx = cv2.approxPolyDP(c, 0.018 * peri, True)

 

 # if our approximated contour has four points, then

 # we can assume that we have found our screen

 if len(approx) == 4:

  screenCnt = approx

  break


 


if screenCnt is None:

 detected = 0

 print ("No contour detected")

else:

 detected = 1


if detected == 1:

 cv2.drawContours(img, [screenCnt], -1, (0, 255, 0), 3)


# Masking the part other than the number plate

mask = np.zeros(gray.shape,np.uint8)

new_image = cv2.drawContours(mask,[screenCnt],0,255,-1,)

new_image = cv2.bitwise_and(img,img,mask=mask)


# Now crop

(x, y) = np.where(mask == 255)

(topx, topy) = (np.min(x), np.min(y))

(bottomx, bottomy) = (np.max(x), np.max(y))

Cropped = gray[topx:bottomx+1, topy:bottomy+1]

 


#Read the number plate 

text = pytesseract.image_to_string(Cropped, config='-l eng --oem 3 --psm 12')

print("Detected Number is:",text)
license_plate = RoLicensePlate(text)
#validare county
print(f"validare judet: {license_plate.validate_county()}")


#mai adauga validari - combinatii de litere care incep cu I sau O / III sau OOO 
print(f"validare litere: {license_plate.validate_letters()}")
print(f"validare numere: {license_plate.validate_number()}")
if license_plate.validate_county() & license_plate.validate_letters(): 
    if license_plate.validate_number():   #daca s-a trecut cu bine de verificarile din functiile validate_county() si validate_Letters()
        print("Nr.inmatriculare valid")                                      #atunci numarul de inmatriculare este valid
else: print("Nr.inmatriculare invalid")
#numere rosii
if license_plate.red_license_plate():
    print("Numar rosu valid")
else:
    print("Numar rosu invalid")
    

#numere diplomatice
if license_plate.diplomatic_license_plate():
    print("Numar diplomatic valid")
else:
    print("Numar diplomatic invalid")
    
    
#numere de inmatriculare rezervate pt institutii de stat
if license_plate.government_licence_plate():
    print("Masina a unei institutii de stat")
    


    
#numerele din Bucuresti au 3 cifre
#pt mai, deschide direct bariera
cv2.imshow('image',img)

cv2.imshow('Cropped',Cropped)


cv2.waitKey(0)

cv2.destroyAllWindows()

