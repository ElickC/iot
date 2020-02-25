from influxdb import InfluxDBClient

import RPi.GPIO as GPIO
import paho.mqtt.client as mqtt
import datetime

broker_address="192.168.1.97"    #broker address (your pis ip address)

#setup board
GPIO.setmode(GPIO.BCM)

#setup pin as output
GPIO.setup(16, GPIO.OUT)

client = mqtt.Client() #create new client instance
client.connect(broker_address) #connect to broker

client.loop_start() #start client

client.subscribe("/light") #subscirbe to topic
client.subscribe("/piled") #subscirbe to topic

payload = ""
topic = ""

def on_message(client, userdata, message):
  print(message.topic + " " + str(message.payload)) #print incoming messages
  payload = message.payload.decode("utf-8")

  if(message.topic == "/piled"):
    if(payload == "on"):
      GPIO.output(16, GPIO.HIGH)
      print("Pi should be on")
    elif(payload == "off"):
      GPIO.output(16, GPIO.LOW)
      print("pi should be off")

  else:
   payload = float(payload)

   #get current time
   receiveTime = datetime.datetime.utcnow()

   # Set up a client for InfluxDB
   dbclient = InfluxDBClient('0.0.0.0', 8086, 'root', 'root', 'mydb')

   #create json to insert into db
   json_body = [
           {
             "measurement": '/light',
   	        "time": receiveTime,
   	        "fields": {
                     "value": payload
   	        }
           }
   ]

   #write to db
   dbclient.write_points(json_body)

client.on_message=on_message #set the on message function

try:
  while True:   # wait for ctrl-c
    pass

except KeyboardInterrupt:
  pass

client.loop_stop() #stop client
