﻿////////////////////////////////////////////////
//
// Demo device emulator for "display-value" widget
//
// 4 different colors demo display-value widgets with random data (colors: red, green, blue, yellow)
//
// IoT Manager https://play.google.com/store/apps/details?id=ru.esp8266.iotmanager
//
// version     : 1.6
// IoT Manager : 1.4.6 and above
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

var deviceID = "uniqueId-0001";
var prefix   = "/IoTmanager";

var config1 = {"id":"1",
  page   : "display 1",
//  pageId : 1,
  widget : "display-value",
  class1 : "item item-dark no-border",
  style1 : "",
  descr  : "Display-1",
  class2 : "balanced",
  style2 : "font-size:20px;float:left;padding-top:10px;font-weight:bold;",
  topic  : prefix + "/" + deviceID + "/display1",
  class3 : "",
  style3 : "float:right;",
  height : "40",
  color  : "#52FF00",
  inactive_color : "#414141",
  digits_count   : 5
};

var config2 = {"id":"2",
  page   : "display 2",
//  pageId : 2,
  widget : "display-value",
  class1 : "item item-dark no-border",
  style1 : "",
  descr  : "Display-2",
  class2 : "assertive",
  style2 : "font-size:20px;float:left;font-weight:bold;",
  topic  : prefix + "/" + deviceID + "/display2",
  class3 : "",
  style3 : "float:right;",
  height : "20",
  color  : "#F10014",
  inactive_color : "#414141",
  digits_count   : 10
};

var config3 = {"id":"3",
  page   : "display 34",
//  pageId : 3,
  widget : "display-value",
  class1 : "item no-border item-dark text-center col-xs-6",
  style1 : "",
  descr  : "d-3",
  class2 : "calm",
  style2 : "font-size:32px;padding-top:10px;padding-bottom:20px;font-weight:bold;",
  topic  : prefix + "/" + deviceID + "/display3",
  class3 : "",
  style3 : "",
  height : "70",
  color  : "#58b7ff",
  inactive_color : "#414141",
  digits_count   : 3
};

var config4 = {"id":"4",
  page   : "display 34",
//  pageId : 3,
  widget : "display-value",
  class1 : "item no-border item-dark text-center col-xs-6",
  style1 : "",
  descr  : "d-4",
  class2 : "energized",
  style2 : "font-size:32px;padding-top:10px;padding-bottom:20px;font-weight:bold;",
  topic  : prefix + "/" + deviceID + "/display4",
  class3 : "",
  style3 : "",
  height : "70",
  color  : "#ffe800",
  inactive_color : "#414141",
  digits_count   : 2
};

var config5 = {

id     :"5",
page   : "Temp",
//pageID : "3",
widget : "display-value",
class1 : "item no-border",                          // class for 1st div
style1 : "",                                        // style for 1st div
descr  : "Комната",                            // text  for description
class2 : "balanced",                                // class for description from Widgets Guide - Color classes
style2 : "font-size:20px;float:left;padding-top:10px;font-weight:bold;", // style for description
topic  : prefix + "/" + deviceID + "/display5",
class3 : "",                                        // class for 3 div - SVG
style3 : "float:right;",                            // style for 3 div - SVG
height : "40",                                      // SVG height without "px"
color  : "#52FF00",                                 // color for active segments
inactive_color : "#414141",                         // color for inactive segments
digits_count   : 4                                 // how many digits
};
var client  = mqtt.connect(opt);

client.on('connect', function () {
  console.log('Broker connected');
  client.subscribe('/IoTmanager',{ qos : 1 });
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
  }
});

console.log('Start');
pubConfig();
setInterval(function() {
  pubStatus1();
  pubStatus2();
  pubStatus3();
  pubStatus4();
  pubStatus5();
}, 5000);

////////////////////////////////////////////////
function pubStatus1() {
      value1 = (20 - Math.round( Math.random() * 40 ));
      console.log("Send status 1:" + value1);
      value1 = value1 + '°C'
      client.publish( config1.topic + "/status", JSON.stringify({ status: value1, toast: "Outdoor temp now: "+value1, vibrate: 50 }) );
}
////////////////////////////////////////////////
function pubStatus2() {
      value2 = (Math.round( Math.random() * 1000000000 ));
      console.log("Send status 2:" + value2);
      client.publish( config2.topic + "/status", JSON.stringify({ status: value2 }) );
}
////////////////////////////////////////////////
function pubStatus3() {
      value3 = (50 - Math.round( Math.random() * 100 ));
      console.log("Send status 3:" + value3);
      client.publish( config3.topic + "/status", JSON.stringify({ status: value3 }) );
}
////////////////////////////////////////////////
function pubStatus4() {
      value4 = Math.round( Math.random() * 99 );
      console.log("Send status 4:" + value4 + " color");
      client.publish( config4.topic + "/status", JSON.stringify({ status: value4, color : "#000000" }) );
}
function pubStatus5() {
      value5 = Math.round( Math.random() * 99 );
      console.log("Send status 5:" + value5 + " color");
      client.publish( config5.topic + "/status", JSON.stringify({ status: value5 }) );
}
////////////////////////////////////////////////
function pubConfig() {
    client.publish(prefix + "/" + deviceID + "/config", JSON.stringify(config1),{ qos : 1 });
    setTimeout(function() {
      pubStatus1();
    }, 200);
    setTimeout(function() {
      client.publish(prefix + "/" + deviceID + "/config", JSON.stringify(config2),{ qos : 1 });
    }, 400);
    setTimeout(function() {
      pubStatus2();
    }, 600);
    setTimeout(function() {
      client.publish(prefix + "/" + deviceID + "/config", JSON.stringify(config3),{ qos : 1 });
    }, 800);
    setTimeout(function() {
      pubStatus3();
    }, 1000);
    setTimeout(function() {
      client.publish(prefix + "/" + deviceID + "/config", JSON.stringify(config4),{ qos : 1 });
    }, 1200);
    setTimeout(function() {
      pubStatus4();
    }, 1400);
      client.publish(prefix + "/" + deviceID + "/config", JSON.stringify(config5),{ qos : 1 });
      pubStatus5();
}
////////////////////////////////////////////////
