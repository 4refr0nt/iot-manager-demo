////////////////////////////////////////////////
//
// Demo device emulator for "fillgauge" widget
//
// 2 tanks and 2 pumps and 4 level sensors.
// If tank have liquid, then you can activate pump for pumping out. After pumping out, pump deactivated automaticaly.
//
// IoT Manager https://play.google.com/store/apps/details?id=ru.esp8266.iotmanager
//
// version     : 1.0
// IoT Manager : 1.4.5 and above
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

var config1 = {"id":"1",
  "page":"control",
  "pageId":"1",
  "descr":"pump 1",
  "widget":"toggle",
  "itemstyle" : "",
  "itemclass" : "",
  "topic":"/IoTmanager/deviceID/pump1",
  "color":"orange",
  "style":""
};
var config2 = {"id":"2",
  "page":"control",
  "pageId":"1",
  "descr":"pump 2",
  "widget":"toggle",
  "style1" : "",
  "class1" : "",
  "topic":"/IoTmanager/deviceID/pump2",
  "color":"blue",
  "style":""
};

var config3 = {"id":"3",
  page   : "levels",
  pageId : "2",
  widget : "fillgauge",
  class1 : "item no-border",
  style1 : "",
  descr  : "Water level",
  class2 : "light text-center",
  style2 : "font-size:24px;font-weight:bold;padding-bottom:5px;",
  topic  : "/IoTmanager/deviceID/fillgauge3",
  width  : "150",
  height : "150",
  class3 : "text-center",
  style3 : "",
  widgetConfig : {
  }
};

var config4 = {"id":"4",
  page   : "levels",
  pageId : "2",
  widget : "fillgauge",
  class1 : "no-border text-center col-xs-4",
  style1 : "",
  descr  : "Fuel level",
  class2 : "assertive text-center",
  style2 : "font-size:14px;padding-bottom:5px;",
  topic  : "/IoTmanager/deviceID/fillgauge4",
  width  : "70px",
  height : "70px",
  class3 : "text-center",
  style3 : "",
  widgetConfig : {
    circleColor : "#FF7777",
    textColor : "#FF4444",
    waveTextColor : "#FFAAAA",
    waveColor : "#FFDDDD",
    circleThickness : 0.2,
    textVertPosition : 0.2,
    waveAnimateTime : 1000
  }
};

var config5 = {
  id        : "5",
  page      : "levels",
  pageId    : "2",
  widget    : "fillgauge",
  class1    : "no-border text-center col-xs-4",
  style1    : "",
  descr     : "Water level",
  class2    : "energized",
  style2    : "font-size:14px;padding-bottom:5px;",
  topic     : "/IoTmanager/deviceID/fillgauge5",
  width     : "70px",
  height    : "70px",
  class3    : "text-center",
  style3    : "",
  widgetConfig : {
    circleColor : "#D4AB6A",
    textColor : "#553300",
    waveTextColor : "#805615",
    waveColor : "#AA7D39",
    circleThickness : 0.1,
    circleFillGap : 0.2,
    textVertPosition : 0.8,
    waveAnimateTime : 2000,
    waveHeight : 0.3,
    waveCount : 1
  }
};

var config6 = {
  id     : "6",
  page   : "levels",
  pageId :  "2",
  widget : "fillgauge",
  class2 : "no-border text-center col-xs-4",
  style1 : "",
  descr  : "Fuel level",
  class2 : "balanced",
  style2 : "font-size:14px;padding-bottom:5px;",
  topic  : "/IoTmanager/deviceID/fillgauge6",
  width  : "70px",
  height : "70px",
  class3 : "text-center",
  style3 : "",
  widgetConfig : {
    circleThickness : 0.15,
    circleColor : "#808015",
    textColor : "#555500",
    waveTextColor : "#FFFFAA",
    waveColor : "#AAAA39",
    textVertPosition : 0.8,
    waveAnimateTime : 1000,
    waveHeight : 0.05,
    waveAnimate : true,
    waveRise : false,
    waveHeightScaling : false,
    waveOffset : 0.25,
    textSize : 0.75,
    displayPercent : false,
    minValue : 30,
    maxValue : 150,
    textSize : 1.2,
    waveCount : 3
  }
};


var volume1 = 0;
var volume2 = 50;
var pump1   = 0;
var pump2   = 0;

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
       if (topic.split("/")[4] == 'control' && topic.split("/")[3] == "pump1") {
          client.publish('/IoTmanager/deviceID/pump1/status',JSON.stringify(status));
          if (status.status == 1) {
             volume1 = 0;
             pump1 = 1
             setTimeout(function() {  // Auto Off after 1sec
                pump1 = 0;
                status = { status : 0 };
                client.publish('/IoTmanager/deviceID/pump1/status',JSON.stringify(status));
             }, 1000);
          } else {
             pump1 = 0
          }
          pubStatus1();
       }
       if (topic.split("/")[4] == 'control' && topic.split("/")[3] == "pump2") {
          client.publish('/IoTmanager/deviceID/pump2/status',JSON.stringify(status));
          if (status.status == 1) {
             volume2 = 0;
             pump2 = 1
             setTimeout(function() {  // Auto Off after 1sec
                pump2 = 0
                status = { status : 0 };
                client.publish('/IoTmanager/deviceID/pump2/status',JSON.stringify(status));
             }, 1000);
          } else {
             pump2 = 0
          }
          pubStatus2();
       }
       console.log("Status (echo) published: " + JSON.stringify(status));
    } else {
       console.log('Message arrived to topic:'+topic.toString()+', payload:'+message.toString());
    }
  }
});

console.log('Start');
////////////////////////////////////////////////
function pubStatus1() {
      console.log("Send status1:" + volume1);
      client.publish('/IoTmanager/deviceID/fillgauge3/status', JSON.stringify({ status: volume1 }));
      client.publish('/IoTmanager/deviceID/fillgauge5/status', JSON.stringify({ status: volume1 }));
}
function pubStatus2() {
      console.log("Send status2:" + volume2);
      client.publish('/IoTmanager/deviceID/fillgauge4/status', JSON.stringify({ status: volume2 }));
      client.publish('/IoTmanager/deviceID/fillgauge6/status', JSON.stringify({ status: volume2 }));
}
////////////////////////////////////////////////
function pubConfig() {
    client.publish('/IoTmanager', 'deviceID');
    client.subscribe('/IoTmanager/deviceID/pump1/control',{ qos : 1 });
    client.subscribe('/IoTmanager/deviceID/pump2/control',{ qos : 1 });
    setTimeout(function() {
      client.publish('/IoTmanager/deviceID/config', JSON.stringify(config1),{ qos : 1 });
      client.publish('/IoTmanager/deviceID/config', JSON.stringify(config2),{ qos : 1 });
      client.publish('/IoTmanager/deviceID/config', JSON.stringify(config3),{ qos : 1 });
      client.publish('/IoTmanager/deviceID/config', JSON.stringify(config4),{ qos : 1 });
      client.publish('/IoTmanager/deviceID/config', JSON.stringify(config5),{ qos : 1 });
      client.publish('/IoTmanager/deviceID/config', JSON.stringify(config6),{ qos : 1 });
      pubStatus1();
      pubStatus2();
    }, 500);
    setInterval(function() {
      if (volume1 < 100) {
         volume1 += 5;
      } else {
         volume1 = 0;
      }
      pubStatus1();

      if (volume2 < 100) {
         volume2 += 5;
      } else {
         volume2 = 0;
      }
      pubStatus2();

    }, 5000);
}
////////////////////////////////////////////////
