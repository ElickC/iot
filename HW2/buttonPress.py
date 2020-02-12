import RPi.GPIO as GPIO
import time

GPIO.setmode(GPIO.BCM)

GPIO.setup(18, GPIO.OUT)

GPIO.setup(21, GPIO.IN, pull_up_down=GPIO.PUD_UP)

clickedflag = False
on = False


while True:
    buttonclick = GPIO.input(21)
    
    if (buttonclick and not clickedflag and not on):
        GPIO.output(18,GPIO.HIGH)
        clickedflag = True
        on = True
        time.sleep(1)
       
    elif (buttonclick and not clickedflag and on):
        GPIO.output(18,GPIO.LOW)
        on = False
        time.sleep(1)
         
    if (not buttonclick and clickedflag):  
        clickedflag = False
        


		