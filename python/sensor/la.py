# iotManager
# load avg sensor
import json
import threading
# import os

frame = "STEEL"    # BLACK_METAL METAL SHINY_METAL BRASS STEEL CHROME GOLD ANTHRACITE TILTED_GRAY TILTED_BLACK GLOSSY_METAL
color = "RAITH" # RED GREEN BLUE ORANGE YELLOW CYAN MAGENTA WHITE GRAY BLACK RAITH GREEN_LCD JUG_GREEN
bgColor = "CARBON" # DARK_GRAY SATIN_GRAY LIGHT_GRAY WHITE BLACK BEIGE BROWN RED GREEN BLUE ANTHRACITE MUD PUNCHED_SHEET CARBON STAINLESS BRUSHED_METAL BRUSHED_STAINLESS TURNED
lcd = "BLUE_BLUE" # BEIGE BLUE ORANGE RED YELLOW WHITE GRAY BLACK GREEN BLUE2 BLUE_BLACK BLUE_DARKBLUE BLUE_GRAY STANDARD STANDARD_GREEN BLUE_BLUE RED_DARKRED DARKBLUE LILA BLACKRED DARKGREEN AMBER LIGHTBLUE SECTIONS
led = "RED_LED" # RED_LED GREEN_LED BLUE_LED ORANGE_LED YELLOW_LED CYAN_LED

class LoadAvg:
  counter = 0
  t = 0
  config = {
    'descr'  : "Load average",
    'widget' : "steel",
    'style1' : "float:left;",
    'widgetConfig' : {
      'titleString' : "Load average 1 min",
      'unitString'  : "%",
      'width'       : "auto2",
      'height'      : 100,
      'type'        : "Linear",
      'lcdVisible'  : True,
      'ledVisible'  : True,
      'lcdDecimals' : 0,
      'FrameDesign' : frame,
      'ColorDef'    : color,
      'BackgroundColor': bgColor,
      'LcdColor'    : lcd,
      'LedColor'    : led,
      'minMeasuredValueVisible' : True,
      'maxMeasuredValueVisible' : True,
      'threshold'   : 50,
      'minValue' : 0,
      'maxValue' : 100,
    }
  }

  def __init__(self, client, prefix, deviceID, widgetID, pageId, page):
    self.client = client
    self.prefix = prefix
    self.deviceID = deviceID
    self.config['id'] = widgetID
    self.config['topic'] = prefix + "/" + deviceID + "/la"
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
      topic = self.prefix + "/" + self.deviceID + "/la/status"
      val = float(self.get()[0]) * 100
      print('Publish ' + topic + ':' + str(val))
      self.client.publish(topic, json.dumps({ 'status': val}) )


  def get(self):
    with open('/proc/loadavg') as f:
      loadavg = f.readlines()
    return str(loadavg[0]).replace('\n', '').split(' ')
