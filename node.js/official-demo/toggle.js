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
var prefix = config.prefix;
var deviceID = "nodejs_emulator"
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
  // min config: green track background without icon, status not visible
  id     :"1",
  descr  : "Light 1",
  widget : "toggle",
  topic  : prefix + "/" + deviceID + "/light1"
};
var config2 = {
  // old-style config with colored icon at left and colored background for ON state
  id     :"2",
  page   :"bathroom",
  pageId :"2",
  descr  :"Light 2",
  widget :"toggle",
  topic  :prefix + "/" + deviceID + "/light2",
  color  :"orange" // also, you can use any HTML color code
};
var config3 = {
  // config with colored description and colored icon
  id     :"3",
  page   :"bathroom",
  pageId :"2",
  descr  :"Light 3",
  widget :"toggle",
  topic  :prefix + "/" + deviceID + "/light3",
  color  : "#00FF00",
  descrStyle : "color:#FFFF00"
};
var config4 = {
  // config without icon at left
  id     :"4",
  page   :"bathroom",
  pageId :"2",
  descr  :"Outdoor light",
  widget :"toggle",
  topic  :prefix+ "/" + deviceID + "/light4",
  iconClass   : "ion-ios-lightbulb",
  iconClassOff: "ion-ios-lightbulb-outline",
  iconStyle   : "font-size:30px;color:#F2CD1F;",
  iconStyleOff: "font-size:30px;color:#eee;",
  descrStyle  : "font-size:20px;line-height:1.5em;"
};
var config5 = {
  // config with colored custom icons and big size description
  id     :"5",
  page   :"bathroom",
  pageId :"2",
  descr  :"Light 5",
  widget :"toggle",
  topic  :prefix + "/" + deviceID + "/light5",
  iconClass   : "ion-battery-charging",
  iconClassOff: "ion-battery-low",
  iconStyle   : "font-size:30px;color:#4cd964;",
  iconStyleOff: "font-size:30px;color:red;",
  descrStyle  : "font-size:20px;line-height:1.5em;"
};
var config6 = {
  // config with custom style description, track and handle
  id     :"6",
  page   :"bathroom",
  pageId :"2",
  descr  :"Light 6",
  widget :"toggle",
  class1 : "",
  topic  :prefix + "/" + deviceID + "/light6",
  descrClass    :"balanced-900 text-center",
  descrStyle    :"background-color:#0F0;border:solid 2px #F00;font-size:20px;font-weight:bold;",
  descrStyleOff :"background-color:#0F0;border:solid 2px #000;font-size:20px;font-weight:bold;",
  iconStyle     :"color:red;font-size:25px;",
  iconStyleOff  :"color:black;font-size:25px;",
  trackStyle    :"background-color:red;",
  trackStyleOff :"background-color:white;",
  handleStyle   :"background-color:yellow;",
  handleStyleOff:"background-color:green;",
};
var config7 = {
  id     :"7",
  page   :"bathroom",
  pageId :"2",
  descr  :"Light 7",
  widget :"toggle",
  topic  :prefix + "/" + deviceID + "/light7",
  descrStyle :"font-size:20px;line-height:1.5em;",
  iconStyle  :"font-size:30px;color:#4cd964;",
};

var client  = mqtt.connect(opt);

client.on('connect', function () {
  console.log('Broker connected');
  client.subscribe(prefix+ "/" + deviceID + '/+/control');
  client.subscribe(prefix);
  pubConfig();
});

client.on('error', function () {
  console.log('error');
});

client.on('offline', function () {
  console.log('offline');
});

client.on('message', function (topic, message) {
  if (topic.toString() == prefix && message.toString() == "HELLO" ){
    console.log('HELLO detected');
    pubConfig();
  } else {
    console.log('message arrived');
    if (topic.split("/")[4] == 'control') {
       console.log('Control message arrived to topic:'+topic.toString()+', payload:'+message.toString());
       var status;
       if (message.toString() === "1") {
         status = { status: 1 };
       } else {
         status = { status: 0 };
       }
       if (topic.split("/")[4] == 'control' && topic.split("/")[3] == "light1") {
          client.publish(prefix + "/" + deviceID + '/light1/status',JSON.stringify(status));
       }
       if (topic.split("/")[4] == 'control' && topic.split("/")[3] == "light2") {
          client.publish(prefix + "/" + deviceID + '/light2/status',JSON.stringify(status));
       }
       if (topic.split("/")[4] == 'control' && topic.split("/")[3] == "light3") {
          client.publish(prefix + "/" + deviceID + '/light3/status',JSON.stringify(status));
       }
       if (topic.split("/")[4] == 'control' && topic.split("/")[3] == "light4") {
          client.publish(prefix + "/" + deviceID + '/light4/status',JSON.stringify(status));
       }
       if (topic.split("/")[4] == 'control' && topic.split("/")[3] == "light5") {
          client.publish(prefix + "/" + deviceID + '/light5/status',JSON.stringify(status));
       }
       if (topic.split("/")[4] == 'control' && topic.split("/")[3] == "light6") {
          client.publish(prefix + "/" + deviceID + '/light6/status',JSON.stringify(status));
       }
       if (topic.split("/")[4] == 'control' && topic.split("/")[3] == "light7") {
          client.publish(prefix + "/" + deviceID + '/light7/status',JSON.stringify(status));
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
    client.publish(prefix + "/" + deviceID + '/config', JSON.stringify(config1));
    client.publish(prefix + "/" + deviceID + '/config', JSON.stringify(config2));
    client.publish(prefix + "/" + deviceID + '/config', JSON.stringify(config3));
    client.publish(prefix + "/" + deviceID + '/config', JSON.stringify(config4));
    client.publish(prefix + "/" + deviceID + '/config', JSON.stringify(config5));
    client.publish(prefix + "/" + deviceID + '/config', JSON.stringify(config6));
    client.publish(prefix + "/" + deviceID + '/config', JSON.stringify(config7));
    console.log('pubConfig:'+prefix + "/" + deviceID + '/config', JSON.stringify(config1));
}
////////////////////////////////////////////////


