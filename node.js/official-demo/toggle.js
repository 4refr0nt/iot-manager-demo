////////////////////////////////////////////////
//
// Demo device emulator for "toggle" widget
//
//
// IoT Manager https://play.google.com/store/apps/details?id=ru.esp8266.iotmanager
//
// version     : 1.2
// IoT Manager : 1.4.11 and above
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
  // min config: green background with green status icon at left
  id     :"1",
  descr  : "Light 1",
  widget : "toggle",
  topic  : "/IoTmanager/deviceID/light1"
};
var config2 = {
  // config with colored icon at left and colored background (only ON state)
  id     :"2",
  page   :"bathroom",
  pageId :"2",
  descr  :"Light 2",
  widget :"toggle",
  topic  :"/IoTmanager/deviceID/light2",
  color  :"orange" // any HTML color code
};
var config3 = {
  // config with colored description (only ON state)
  id     :"3",
  page   :"bathroom",
  pageId :"2",
  descr  :"Light 3",
  widget :"toggle",
  topic  :"/IoTmanager/deviceID/light3",
  widgetConfig : {
     color     : "#FFFF00",
     descrColor: "#FFFF00",
     iconColor : "red",
  }
};
var config4 = {
  // config without icon at left
  id     :"4",
  page   :"bathroom",
  pageId :"2",
  descr  :"Light 4",
  widget :"toggle",
  topic  :"/IoTmanager/deviceID/light4",
  widgetConfig : {
        color  : "orange",
        noIcon : true,
        descrColor: "red"
  }
};
var config5 = {
  // config with colored custom icons
  id     :"5",
  page   :"bathroom",
  pageId :"2",
  descr  :"Light 5",
  widget :"toggle",
  style2 :"line-height:2em;",
  topic  :"/IoTmanager/deviceID/light5",
  widgetConfig : {
        color  : "red",
        iconOn : "ion-battery-charging",
        iconOff: "ion-battery-low",
        iconSize: "30px"
  }
};
var config6 = {
  // config without icon at left
  id     :"6",
  page   :"bathroom",
  pageId :"2",
  style1 :"background-color:#0F0",
  descr  :"Light 6",
  class2 :"balanced",
  style2 :"font-size:20px;font-weight:bold;",
  widget :"toggle",
  topic  :"/IoTmanager/deviceID/light6",
  widgetConfig : {
        color  : "blue",
        descrColor :"red",
        style3 :"background-color:#FFF;border:solid 2px #0F0"
  }
};
var config7 = {
  id     :"7",
  page   :"bathroom",
  pageId :"2",
  descr  :"Light 7",
  widget :"toggle",
  style2 :"font-size:20px;line-height:2em;",
  topic  :"/IoTmanager/deviceID/light7",
  widgetConfig : {
        color  : "green",
        descrColor :"red",
        iconSize: "40px"
  }
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
       if (topic.split("/")[4] == 'control' && topic.split("/")[3] == "light3") {
          client.publish('/IoTmanager/deviceID/light3/status',JSON.stringify(status));
       }
       if (topic.split("/")[4] == 'control' && topic.split("/")[3] == "light4") {
          client.publish('/IoTmanager/deviceID/light4/status',JSON.stringify(status));
       }
       if (topic.split("/")[4] == 'control' && topic.split("/")[3] == "light5") {
          client.publish('/IoTmanager/deviceID/light5/status',JSON.stringify(status));
       }
       if (topic.split("/")[4] == 'control' && topic.split("/")[3] == "light6") {
          client.publish('/IoTmanager/deviceID/light6/status',JSON.stringify(status));
       }
       if (topic.split("/")[4] == 'control' && topic.split("/")[3] == "light7") {
          client.publish('/IoTmanager/deviceID/light7/status',JSON.stringify(status));
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
    client.publish('/IoTmanager/deviceID/config', JSON.stringify(config2));
    client.publish('/IoTmanager/deviceID/config', JSON.stringify(config3));
    client.publish('/IoTmanager/deviceID/config', JSON.stringify(config4));
    client.publish('/IoTmanager/deviceID/config', JSON.stringify(config5));
    client.publish('/IoTmanager/deviceID/config', JSON.stringify(config6));
    client.publish('/IoTmanager/deviceID/config', JSON.stringify(config7));
}
////////////////////////////////////////////////

