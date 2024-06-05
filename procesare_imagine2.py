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

sharp_ker = np.array([[0, -1, 0], [-1, 5, -1], [0, -1, 0]])

# Start the camera preview
camera.start_preview(Preview.QTGL)
camera.start()

try:
    while True:
        time.sleep(10)  # Delay for camera stabilization and refresh
        camera.capture_file("alabala.jpg")
        
        img = cv2.imread("alabala.jpg")
        img = cv2.resize(img, (1920, 640))
        img = cv2.filter2D(img, -1, sharp_ker)
        
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        gray = cv2.bilateralFilter(gray, 11, 17, 17)
        edged = cv2.Canny(gray, 30, 200)
        
        cnts = cv2.findContours(edged.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
        cnts = imutils.grab_contours(cnts)
        cnts = sorted(cnts, key=cv2.contourArea, reverse=True)[:10]
        screenCnt = None
        
        for c in cnts:
            peri = cv2.arcLength(c, True)
            approx = cv2.approxPolyDP(c, 0.018 * peri, True)
            if len(approx) == 4:
                screenCnt = approx
                break
        
        if screenCnt is None:
            print("No contour detected")
            continue
        
        mask = np.zeros(gray.shape, np.uint8)
        new_image = cv2.drawContours(mask, [screenCnt], 0, 255, -1,)
        new_image = cv2.bitwise_and(img, img, mask=mask)
        
        (x, y) = np.where(mask == 255)
        (topx, topy) = (np.min(x), np.min(y))
        (bottomx, bottomy) = (np.max(x), np.max(y))
        Cropped = gray[topx:bottomx+1, topy:bottomy+1]
        
        text = pytesseract.image_to_string(Cropped, config='-l eng --oem 3 --psm 8')
        print("Detected Number is:", text)
        if(len(text) == 6 or len(text)==5):
         break  # Valid plate found, exit the loop
        
        # Additional conditions can be added here if necessary

except KeyboardInterrupt:
    print("Interrupted by user")

finally:
    license_plate = RoLicensePlate(text)
    if license_plate.validate_county() and license_plate.validate_letters() and license_plate.validate_number():
         print("License plate is valid")
    camera.stop_preview()
    camera.stop()
    cv2.destroyAllWindows()
