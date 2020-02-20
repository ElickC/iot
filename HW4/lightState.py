from influxdb import InfluxDBClient

import RPi.GPIO as GPIO
import paho.mqtt.client as mqtt
import datetime

broker_address="192.168.1.18"    #broker address (your pis ip address)

def on_message(client, userdata, message):
  print(message.topic + " " + str(message.payload)) #print incoming messages

  payload = message.payload.decode("utf-8")
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
  print("Finished writing to InfluxDB")


client = mqtt.Client() #create new client instance
client.connect(broker_address) #connect to broker

client.on_message=on_message #set the on message function

client.subscribe("/light") #subscirbe to topic

client.loop_start() #start client

#setup board
GPIO.setmode(GPIO.BCM)

#setup pin as output
GPIO.setup(18, GPIO.OUT)

try:
  while True:   # wait for ctrl-c

    # Set up a client for InfluxDB
    dbclient = InfluxDBClient('0.0.0.0', 8086, 'root', 'root', 'mydb')

    #database query
    query = 'select mean("value") from "/light" where "time" > now() - 10s'

    #make query
    result = dbclient.query(query)

    try:

      light_avg = list(result.get_points(measurement='/light'))[0]['mean']
      print("Average is: " , light_avg)
      if light_avg <= 200:
          GPIO.output(18, GPIO.HIGH)
      else:
          GPIO.output(18, GPIO.LOW)

    except:
      print('exception')
      pass

except KeyboardInterrupt:
  pass

client.loop_stop() #stop client
