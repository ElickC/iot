from influxdb import InfluxDBClient
from flask import Flask, request, json
from flask_restful import Resource, Api

import RPi.GPIO as GPIO
import paho.mqtt.client as mqtt
import datetime

broker_address="192.168.1.97"    #broker address (your pis ip address)

def on_message(client, userdata, message):
  #print(message.topic + " " + str(message.payload)) #print incoming messages

  payload = message.payload.decode("utf-8")

  if(topic == "/piled"):
        if(payload == "on"):
            GPIO.output(18,GPIO.HIGH)
        elif(payload == "off"):
            GPIO.output(18,GPIO.LOW)

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
  #print("Finished writing to InfluxDB")


client = mqtt.Client() #create new client instance
client.connect(broker_address) #connect to broker

client.on_message=on_message #set the on message function

client.subscribe("/light") #subscirbe to topic
client.subscribe("/piled") #subscribe to led topic
client.subscribe("/arled") #subscribe to led topic

client.loop_start() #start client

#setup board
GPIO.setmode(GPIO.BCM)

#setup pin as output
GPIO.setup(18, GPIO.OUT)

app = Flask(__name__)
api = Api(app)


class lightPrint(Resource):
    def get(self):
      # Set up a client for InfluxDB
      dbclient = InfluxDBClient('0.0.0.0', 8086, 'root', 'root', 'mydb')

      #database query
      query = 'select mean("value") from "/light" where "time" > now() - 10s'

      #make query
      result = dbclient.query(query)

      try:
        light_avg = list(result.get_points(measurement='/light'))[0]['mean']
        return(light_avg)

      except:
        #print('exception')
        pass

class lightChange(Resource):
    def post(self):
      value = request.get_data()
      value = json.loads(value)
      print(value)
      if value["device"] == "pi":
        if value["state"] == "on":
          print("In pi, on")
          client.publish("/piled","on") #send message
        else:
          client.publish("/piled","off") #send message
      if value["device"] == "esp":
        if value["state"] == "on":
          client.publish("/arled","on") #send message
        else:
          client.publish("/arled","off") #send message

      return {"device":value["device"], "state":value["state"]}


api.add_resource(lightPrint, '/light')
api.add_resource(lightChange, '/')

app.run(host='0.0.0.0', debug=True)


client.loop_stop() #stop client
