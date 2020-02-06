import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)

GPIO.setup(18, GPIO.OUT)

GPIO.setup(21, GPIO.IN, pull_up_down=GPIO.PUD_UP)

clickedflag = False

while True:
    buttonstate = GPIO.input(21)

    if (buttonstate and not clickedflag):
        GPIO.output(18,GPIO.HIGH)
        clickedflag = True
    if (not buttonstate and clickedflag):
        GPIO.output(18,GPIO.LOW)
        clickedflag = False


		
