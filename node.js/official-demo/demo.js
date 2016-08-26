////////////////////////////////////////////////
//
// Demo device emulator for "toggle" widget
//
//
// IoT Manager https://play.google.com/store/apps/details?id=ru.esp8266.iotmanager
//
// version     : 1.1
// IoT Manager : 1.4.1 and above
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

var config1 = {
  id     :"1",
  page   : "kitchen",
  pageId : "1",
  descr  : "Light 1",
  widget : "toggle",
  topic  : "/IoTmanager/deviceID/light1",
  color  : "orange",
  style1 : ""
};
var config2 = {
  id     :"2",
  page   :"bathroom",
  pageId :"2",
  descr  :"Light 2",
  widget :"toggle",
  topic  :"/IoTmanager/deviceID/light2",
  color  :"blue",
  style  :""
};

var client  = mqtt.connect(opt);

client.on('connect', function () {
  console.log('Broker connected');
  client.subscribe('/IoTmanager/deviceID/+/control');
  client.subscribe('/IoTmanager');
  pubConfig();
});

client.on('error', function () {
  console.log('error');
});

client.on('offline', function () {
  console.log('offline');
});

client.on('message', function (topic, message) {
  if (topic.toString() == "/IoTmanager" && message.toString() == "HELLO" ){
    console.log('HELLO detected');
    pubConfig();
  } else {
    if (topic.split("/")[4] == 'control') {
       console.log('Control message arrived to topic:'+topic.toString()+', payload:'+message.toString());
       var status;
       if (message.toString() === "1") {
         status = { status: 1 };
       } else {
         status = { status: 0 };
       }
       if (topic.split("/")[4] == 'control' && topic.split("/")[3] == "light1") {
          client.publish('/IoTmanager/deviceID/light1/status',JSON.stringify(status));
       }
       if (topic.split("/")[4] == 'control' && topic.split("/")[3] == "light2") {
          client.publish('/IoTmanager/deviceID/light2/status',JSON.stringify(status));
       }
       console.log("Status (echo) published: " + JSON.stringify(status));
    } else {
       console.log('Message arrived to topic:'+topic.toString()+', payload:'+message.toString());
    }
  }
});

console.log('Start');
////////////////////////////////////////////////
function pubConfig() {
    client.publish('/IoTmanager/deviceID/config', JSON.stringify(config1));
    setTimeout(function() {
      client.publish('/IoTmanager/deviceID/config', JSON.stringify(config2));
    }, 200);
}
////////////////////////////////////////////////