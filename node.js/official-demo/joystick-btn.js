////////////////////////////////////////////////
//
// Demo device emulator for "joystick-btn" widget
//
//
//
// IoT Manager https://play.google.com/store/apps/details?id=ru.esp8266.iotmanager
//
// version     : 1.0
// IoT Manager : 1.4.7 and above
//
////////////////////////////////////////////////

////////////////////////////////////////////////
var config = require("./config");
var host = config.host;
var port = config.port;
var user = config.user;
var pass = config.pass;
////////////////////////////////////////////////

var mqtt = require('mqtt');
var opt = {
  host       : host,
  port       : port,
  username   : user,
  password   : pass,
  clientId   : 'mqttjs_' + Math.random().toString(16).substr(2, 8),
  protocolId : 'MQTT',
  connectTimeout: 3000
};

var deviceID = "uniqueId-0002";
var prefix   = "/IoTmanager";
var widget   = "joystick-btn4";

var config1 = {"id":"1",
  page   : "joystick",
  widget : widget,
  class1 : "item no-border no-padding",
  style1 : "",
  descr  : "GamePad",
  class2 : "text-center balanced",
  style2 : "font-size:20px;font-weight:bold;",
  topic  : prefix + "/" + deviceID + "/" + widget,
  class3 : "text-center",
  style3 : "",
  widgetConfig : {
    fill          : "#DDE1DD",
    fillPressed   : "#101010",
    arrow         : "#4990E2",
    arrowPressed  : "#A6CAF3",
    delay         : 200
  }
};


var client  = mqtt.connect(opt);

client.on('connect', function () {
  console.log('Broker connected');
  client.subscribe(prefix, { qos : 1 });
  client.subscribe(prefix + "/" + deviceID + "/" + widget + "/control",{ qos : 1 });
  pubConfig();
});

client.on('error', function () {
  console.log('error');
});

client.on('offline', function () {
  console.log('offline');
});

client.on('message', function (topic, message) {
  if (topic.toString() === prefix && message.toString() == "HELLO" ){
    console.log('HELLO detected');
    pubConfig();
  } else {
    console.log("msg: " + topic.toString() + " => " + message.toString());
  }
});

console.log('Start');
pubConfig();

////////////////////////////////////////////////
function pubConfig() {
    client.publish( prefix, deviceID );
    setTimeout(function() {
      client.publish(prefix + "/" + deviceID + "/config", JSON.stringify(config1),{ qos : 1 });
    }, 500);
}
////////////////////////////////////////////////
