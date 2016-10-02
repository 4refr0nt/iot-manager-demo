# iotManager
# RPI memory sensor
# @auhtor Alex Suslov <https://github.com/alexsuslov>
# @created 2016-09-06
# The MIT License (MIT)
# Copyright (c) 2016 AlexSuslov

import json
import threading

frame = "STEEL"    # BLACK_METAL METAL SHINY_METAL BRASS STEEL CHROME GOLD ANTHRACITE TILTED_GRAY TILTED_BLACK GLOSSY_METAL
color = "RAITH" # RED GREEN BLUE ORANGE YELLOW CYAN MAGENTA WHITE GRAY BLACK RAITH GREEN_LCD JUG_GREEN
bgColor = "CARBON" # DARK_GRAY SATIN_GRAY LIGHT_GRAY WHITE BLACK BEIGE BROWN RED GREEN BLUE ANTHRACITE MUD PUNCHED_SHEET CARBON STAINLESS BRUSHED_METAL BRUSHED_STAINLESS TURNED
lcd = "BLUE_BLUE" # BEIGE BLUE ORANGE RED YELLOW WHITE GRAY BLACK GREEN BLUE2 BLUE_BLACK BLUE_DARKBLUE BLUE_GRAY STANDARD STANDARD_GREEN BLUE_BLUE RED_DARKRED DARKBLUE LILA BLACKRED DARKGREEN AMBER LIGHTBLUE SECTIONS
led = "RED_LED" # RED_LED GREEN_LED BLUE_LED ORANGE_LED YELLOW_LED CYAN_LED

class Memory:
  counter = 0
  t = 0
  config = {
    'descr'  : "Memory usage",
    'widget' : "fillgauge",
    'class1' : "no-border text-center col-xs-6",
    'class2' : "calm",
    'style2' : "font-size:14px;padding-bottom:5px;",
    'width'  : "150px",
    'height' : "150px",
    'class3' : "text-center",
    'widgetConfig' : {
	    'circleThickness' : 0.1,
	    'waveAnimateTime' : 1000,
	    'waveHeight'      : 0.05,
	    'waveAnimate'     : 'true',
	    'waveOffset'      : 0.25,
	    'textSize'        : 0.6,
	    'minValue'        : 0,
	    'maxValue'        : 100,
	    'waveCount'       : 2
    }
  }

  def __init__(self, client, prefix, deviceID, widgetID, pageId, page):
    self.client = client
    self.prefix = prefix
    self.deviceID = deviceID
    self.config['id'] = widgetID
    self.config['topic'] = prefix + "/" + deviceID + "/mem"
    self.config['pageId'] = pageId
    self.config['page'] = page
    self.t = threading.Timer(10.0, self.send)
    self.t.start()

  def send_config(self):
    print('Publish config:'+ json.dumps(self.config))
    self.client.publish( self.prefix + "/" + self.deviceID + '/config', json.dumps(self.config))
    self.counter = 5 * 60 / 10 # 5 min send
    x = threading.Timer(2.0, self.send)
    x.start()

  def send(self):
    self.t.cancel()
    self.t = threading.Timer(10.0, self.send)
    self.t.start()
    if(self.counter > 0):
      self.counter = self.counter - 1
      topic = self.prefix + "/" + self.deviceID + "/mem/status"
      print('Publish ' + topic + ':' + str(self.get()["p_MemUsed"]))
      self.client.publish(topic, json.dumps({ 'status': self.get()["p_MemUsed"]}) )

  def get(self):
    resp = {}
    with open('/proc/meminfo') as f:
      for line in f:
        data = line.replace("\n", "").replace("kB", "").split(":")
        resp[data[0]] = int(data[1].strip())

    resp['MemUsed'] = resp['MemTotal'] - resp['MemFree']
    resp['p_MemUsed'] = (resp['MemUsed'] / resp['MemTotal']) * 100
    return resp