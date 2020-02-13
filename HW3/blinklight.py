import paho.mqtt.client as mqtt
import RPi.GPIO as GPIO
import time

broker_address="192.168.4.25" #broker address (your pis ip address)

client = mqtt.Client() #create new mqtt client instance

client.connect(broker_address) #connect to broker

client.loop_start() #start client

client.subscribe("/piled") #subscribe to led topic
client.subscribe("/arled") #subscribe to led topic

payload = ""
topic = ""

def on_subscribe(client, userdata, mid, granted_qos):
    print("I've subscribed")

def on_message(client, userdata, msg):
    print("Message received. Topic: {}. Payload: {}".format(msg.topic, str(msg.payload)))
    payload = str(msg.payload)
    topic = msg.topic
    print(payload)
    print(topic)
    payload = msg.payload.decode("utf-8")
    print(payload)
    if(topic == "/piled"):
        if(payload == "on"):
            GPIO.output(18,GPIO.HIGH)
        elif(payload == "off"):
            GPIO.output(18,GPIO.LOW)


client.on_message = on_message

GPIO.setmode(GPIO.BCM)

GPIO.setup(18, GPIO.OUT)

GPIO.setup(21, GPIO.IN, pull_up_down=GPIO.PUD_UP)

clickedflag = False
lighton = False


while True:
    buttonclick = GPIO.input(21)

    if (buttonclick and not clickedflag and not lighton):
        client.publish("/arled","on") #send message
        clickedflag = True
        lighton = True
        time.sleep(1)

    elif (buttonclick and not clickedflag and lighton):
        client.publish("/arled","off") #send message
        lighton = False
        time.sleep(1)

    if (not buttonclick and clickedflag):
        clickedflag = False


client.loop_stop() #stop client
