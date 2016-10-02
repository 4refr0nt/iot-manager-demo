# iotManager
# RPI
# @auhtor Alex Suslov <https://github.com/alexsuslov>
# @created 2016-09-06
# The MIT License (MIT)
# Copyright (c) 2016 AlexSuslov

import os
import paho.mqtt.client as mqtt
import json
import threading
import httplib2
from sensor.temp import Temperature
from sensor.la import LoadAvg
from sensor.la5 import LoadAvg5
from sensor.la15 import LoadAvg15
from sensor.mem import Memory
from sensor.sdcard import SDcard

connection = False
ids = [""]
# mqtt config
host = "m20.cloudmqtt.com"
port = 13191
username = "test"
password = "test"

# IoT MAnager config
prefix   = '/IoTmanager'
deviceID = 'rpi';

# mqtt
client = mqtt.Client()

temperature = Temperature(client, prefix, deviceID, 1, 1, 'page1')
loadavg = LoadAvg(client, prefix, deviceID, 2, 2, 'page2')
loadavg5 = LoadAvg5(client, prefix, deviceID, 3, 2, 'page2')
loadavg15 = LoadAvg15(client, prefix, deviceID, 4, 3, 'page3')
mem = Memory(client, prefix, deviceID, 5, 3, 'page3')
sdcard = SDcard(client, prefix, deviceID, 6, 3, 'page3')


def send_configs():
  print("Sending all configs ...")
  temperature.send_config()
  loadavg.send_config()
  loadavg5.send_config()
  loadavg15.send_config()
  mem.send_config()
  sdcard.send_config()

def on_connect(client, userdata, flags, rc):
  print("Connected with result code " + str(rc))
  client.subscribe(prefix)
  client.subscribe(prefix + "/#")
  send_configs()

def on_message(client, userdata, msg):
  # topic
  t = msg.topic
  # message
  m = msg.payload.decode("utf-8")
  print("Message arrived: " + t + "=>" + m)

  if( t == prefix and m == u'HELLO'):
    print("HELLO detected")
    send_configs()

  if (t == prefix + "/push" and ids[0] != ""):
    http = httplib2.Http()
    url= "https://onesignal.com/api/v1/notifications"
    headers = {'Content-type': 'application/json'}
    data = {
      "app_id": "8871958c-5f52-11e5-8f7a-c36f5770ade9",
      "include_player_ids":ids,
      "android_group":"IoT Manager",
      "contents": {
        "en": m
      }
    }
    response, content = http.request(url, 'POST', headers=headers, body=json.dumps(data))
    print(response.status, response.reason)

  if(t == (prefix + "/ids")):
    print("ids detected:" + m)
    ids[0] = m
    # store ids for sending alerts
    # ids = m ...

client.on_connect = on_connect
client.on_message = on_message
client.username_pw_set(username, password)
print("Try connecting to " + host + ":" + str(port))
client.connect(host, port, 60)
client.loop_forever()

