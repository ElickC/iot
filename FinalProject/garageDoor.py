# Elick Coval
# IoT @ UML 2020

import paho.mqtt.client as mqtt
import RPi.GPIO as GPIO
import time

broker_address="192.168.1.142" #broker address (your pis ip address)

client = mqtt.Client() #create new mqtt client instance

client.connect(broker_address) #connect to broker

client.loop_start() #start client

client.subscribe("/dogDoor") #subscribe to topic

payload = ""
topic = ""

def on_subscribe(client, userdata, mid, granted_qos):
    print("I've subscribed")

def on_message(client, userdata, msg):
    print("Message received. Topic: {}. Payload: {}".format(msg.topic, str(msg.payload)))
    payload = str(msg.payload)
    topic = msg.topic
    payload = msg.payload.decode("utf-8")
    if(topic == "/dogDoor"):
      if(payload == "activateDoor"):
        print("Pi Opening Door")
        GPIO.output(16, GPIO.HIGH)
      # elif(payload == "closeDoor"):
      #   print("***Pi Closing Door***")
      #   GPIO.output(16, GPIO.LOW)


client.on_message = on_message

GPIO.setmode(GPIO.BCM)

# set up relay
GPIO.setup(16, GPIO.OUT)

# set up infrared sensor
GPIO.setup(21, GPIO.IN, pull_up_down=GPIO.PUD_UP)

timesDogPassed = 0;

while True:
    laserIntact = GPIO.input(21)

    if not laserIntact:
        timesDogPassed += 1
        print("Laser Broken")
        time.sleep(1)

    if timesDogPassed == 2:
        print("Close Door")
        GPIO.output(16, GPIO.HIGH)
        timesDogPassed = 0
        time.sleep(1)




client.loop_stop() #stop client
