# iotManager
# RPI Temperature sensor
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

class Temperature:
  counter = 0
  t = 0
  config = {
    'descr'  : "Temp",
    'widget' : "steel",
    'widgetConfig': {
       'titleString' : "Temp",
       'unitString'  : "C",
       'type'        : "LinearBargraph",
       'width'       : "auto",
       'height'      : 100,
       'useValueGradient': True,
       'lcdVisible'  : True,
       'ledVisible'  : True,
       'lcdDecimals' : 1,
       'FrameDesign' : frame,
       'ColorDef'    : color,
       'BackgroundColor': bgColor,
       'LcdColor'    : lcd,
       'LedColor'    : led,
       'maxMeasuredValueVisible' : True,
       'threshold'   : 70,
       'minValue' : 20,
       'maxValue' : 85
    }
  }

  def __init__(self, client, prefix, deviceID, widgetID, pageId, page):
    self.client = client
    self.prefix = prefix
    self.deviceID = deviceID
    self.config['id'] = widgetID
    self.config['topic'] = prefix + "/" + deviceID + "/temp"
    self.config['pageId'] = pageId
    self.config['page'] = page
    self.t = threading.Timer(10.0, self.send)
    self.t.start()

  def send_config(self):
    print('Publish config:'+ json.dumps(self.config))
    self.client.publish( self.prefix + "/" + self.deviceID + '/config', json.dumps(self.config))
    self.counter = 5 * 60 / 10 # 5 min send
    self.send()

  def send(self):
    self.t.cancel()
    self.t = threading.Timer(10.0, self.send)
    self.t.start()
    if(self.counter > 0):
      self.counter = self.counter - 1
      topic = self.prefix + "/" + self.deviceID + "/temp/status"
      print('Publish ' + topic + ':' + str(self.get()))
      self.client.publish(topic, json.dumps({ 'status': self.get()}) )

  def get(self):
    with open('/sys/class/thermal/thermal_zone0/temp') as f:
      temp = f.readlines()
    return  1/1000 * int( str(temp[0]) )