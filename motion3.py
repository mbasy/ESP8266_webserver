import RPi.GPIO as GPIO
import time
from datetime import datetime

GPIO.setmode(GPIO.BCM)
right = 4
left = 24
right_high = False
left_high = False
people = 0

print("Setup complete...")
print("Starting script")

try:
    while True:
              GPIO.setup(right, GPIO.IN)
              GPIO.setup(left, GPIO.IN)
              if GPIO.input(right):
                                     right_high = True
                                     print(datetime.now(), " Motion Detected, right!")
                                     time.sleep(1)
                                     if GPIO.input(left) and right_high:
                                         print("<<<<\n")
                                         people+=1
                                         time.sleep(3)
                                         right_high = False
              elif GPIO.input(left):
                                     left_high = True
                                     print(datetime.now(), " Motion Detected, left!")
                                     time.sleep(1)
                                     if GPIO.input(right) and left_high:
                                         print(">>>>\n")
                                         people-=1
                                         time.sleep(3)
                                         left_high = False
              else:
                   print(datetime.now(), "\n\n\n\n\n\n\n\n\n\n\nNO MOTION, # of people inside:", people)
              time.sleep(0.5)
except KeyboardInterrupt:
    print("Quitting")
    GPIO.cleanup()
