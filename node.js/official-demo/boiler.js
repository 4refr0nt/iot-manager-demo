////////////////////////////////////////////////
//
// Demo device emulator "Boiler: thermostat with adjustable hysteresis"
//
//
//
// IoT Manager https://play.google.com/store/apps/details?id=ru.esp8266.iotmanager
//
// version     : 1.0
// IoT Manager : 1.4.8 and above
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
  clientId   : 'mqtt-js_' + Math.random().toString(16).substr(2, 8),
  protocolId : 'MQTT',
  connectTimeout: 3000
};


var heater = false;
var waterTemp = 20;
var hiTemp    = 40;
var hysteresis= 5;
var lowTemp   = hiTemp - hysteresis;
var manualStop= false;
var lastHeater= true;
var lastAlert = true

var deviceID = "boiler-0001";
var prefix   = "/IoTmanager";
var config   = [];
var client   = mqtt.connect(opt);

// First line
var widget   = "anydata";
var id       = "0"
config[0] = {
  id     : id,
  page   : "boiler",
  pageId : 1,
  widget : widget,
  class1 : "item no-border",
  style2 : "font-size:16px;",
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class3 : "calm text-center",
  style3 : "font-size:20px;",
  status : "Boiler"
};


// -
widget    = "simple-btn";
id        = "1"
config[1] = {
  id     : id,
  page   : "boiler",
  pageId : 1,
  widget : widget,
  class1 : "col-xs-4",
  class2 : "calm",
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class3 : "button button-calm icon ion-minus",
  style3 : "height:70px;",
};

// temp
widget    = "display-value";
id        = "2"
config[2] = {
  id     : id,
  page   : "boiler",
  pageId : 1,
  widget : widget,
  class1 : "no-padding-left col-xs-4",
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  height : "70",
  color  : "#58b7ff",
  inactive_color : "#414141",
  digits_count   : 2
};

// +
widget    = "simple-btn";
id        = "3"
config[3] = {
  id     : id,
  page   : "boiler",
  pageId : 1,
  widget : widget,
  class1 : "col-xs-4",
  style1 : "",
  class2 : "calm",
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class3 : "button button-calm icon ion-plus",
  style3 : "height:70px;",
};

// Current temp
widget    = "anydata";
id        = "4"
config[4] = {
  id     : id,
  page   : "boiler",
  pageId : 1,
  widget : widget,
  class1 : "item no-border",
  style2 : "font-size:16px;float:left",
  descr  : "Current water temp",
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class3 : "assertive",
  style3 : "font-size:40px;font-weight:bold;float:right",
};

// Heater status
widget    = "anydata";
id        = "5"
config[5] = {
  id     : id,
  page   : "boiler",
  pageId : 1,
  widget : widget,
  class1 : "item no-border",
  style2 : "font-size:16px;float:left",
  descr  : "Heater status",
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class3 : "light padding-left padding-right rounded",
  style3 : "font-size:20px;font-weight:bold;float:right",
};

// Stop
widget    = "simple-btn";
id        = "6"
config[6] = {
  id     : id,
  page   : "boiler",
  pageId : 1,
  widget : widget,
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class1 : "item no-border padding-bottom",
  descr  : "Emergency Stop heating",
  class2 : "assertive padding-top",
  style2 : "float:left;",
  class3 : "button icon ion-checkmark-circled",
  style3 : "float:right;",
  widgetConfig : {
    fill : "#FF5050",
    fillPressed : "#FF7070",
    label: "#FFFFFF",
    labelPressed: "#000000",
    alertText     : "A you sure?", // confirmation will be show after button pressed
    alertTitle    : "Stop heating", 
  }
/*
  widgetConfig : {
    fill          : "#FF5050",     // You can use any HTML colors
    fillPressed   : "#00FF00",
    labelPressed  : "#0000FF",
    label         : "#FFFFFF",
    title         : "1",
    alertText     : "A you sure?", // confirmation will be show after button pressed
    alertTitle    : "Stop heating", 
  }
*/
};


client.on('connect', function () {
  console.log('Broker connected');
  client.subscribe(prefix, { qos : 1 }); // HELLO expected
  client.subscribe(prefix + "/" + deviceID +"/+/control", { qos : 1 }); // all command
  pubConfig();
});

client.on('error', function () {
  console.log('error');
});

client.on('offline', function () {
  console.log('! offline');
});

client.on('message', function (topic, message) {
  
  //console.log("msg: " + topic.toString() + " => " + message.toString());

  if (topic.toString() === prefix && message.toString() == "HELLO" ){
    console.log('HELLO detected');
    pubConfig();
  }
  if (topic.toString() === config[1].topic + "/control" ){ 
    console.log("Receive command: max temp dec");

    hiTemp  = hiTemp - 1;
    lowTemp = lowTemp - 1;
    
    manualStop = false;
    pubControlTempStatus();
  }
  if (topic.toString() === config[3].topic + "/control" ){ 
    console.log("Receive command: max temp inc");

    hiTemp = hiTemp + 1;
    lowTemp = lowTemp + 1;

    console.log("New temp:" + hiTemp);
    if (hiTemp >= 90) {  // do not overheat
       hiTemp = 90;
    }
    if (manualStop) {
       client.publish(config[6].topic + "/status", JSON.stringify(config[6]),{ qos : 1 });
    }

    manualStop = false;
    pubControlTempStatus();
  }
  if (topic.toString() === config[6].topic + "/control" ){ 
    console.log("!!! Receive command: manualStop heater control");

    var newStyle = {
        descr  : "Heating already stopped",
//        class3 : "button button-light icon ion-close-circled"
    }
    client.publish(config[6].topic + "/status", JSON.stringify(newStyle),{ qos : 1 });

    manualStop = true;
    heater = false;
    pubControlTempStatus();
    pubHeaterStatus();
  }
});
////////////////////////////////////////////////
function pubConfig() {
    client.publish( prefix, deviceID );
    config.forEach(function(item, i, arr) {
      client.publish(prefix + "/" + deviceID + "/config", JSON.stringify(item),{ qos : 1 });
    });    
    setTimeout(function() {
       pubControlTempStatus();
       pubHeaterStatus(true);
    }, 500);
}

////////////////////////////////////////////////
function pubAlert() {
   widget = "anydata";
   id     = "7"
   var cfgAlert = {
     id     : id,
     page   : "boiler",
     pageId : 1,
     widget : widget,
     class1 : "item rounded text-center no-padding",
     class2 : "assertive-bg light",
     style2 : "font-size:20px;font-weight:bold",
     descr  : "FREEZE ALERT !!!",
     topic  : prefix + "/" + deviceID + "/" + widget + id
   };  
   cfgNoAlert = {
     id     : id
   };
   if (waterTemp <= 5) {
      if (waterTemp < 0) waterTemp = 0;
      if (lastAlert) {
          client.publish(prefix + "/" + deviceID + "/config", JSON.stringify(cfgAlert),{ qos : 1 });
          console.log("Pub Alert!")
          lastAlert = !lastAlert
      }
   } else {
      if (!lastAlert) {
          client.publish(prefix + "/" + deviceID + "/config", JSON.stringify(cfgNoAlert),{ qos : 1 });
          console.log("Pub no alert")
          lastAlert = !lastAlert
      }
   }
}
////////////////////////////////////////////////
function pubWaterTempStatus() {
  client.publish( config[4].topic+"/status", JSON.stringify({ status: waterTemp + "°C" }) );
}
////////////////////////////////////////////////
function pubControlTempStatus() {
  client.publish( config[2].topic+"/status", JSON.stringify({ status: hiTemp }) );
}
////////////////////////////////////////////////
function pubHeaterStatus( force ) {
  if (lastHeater !== heater || force) {
        var newStyle;
        var newStyle1;
        if (heater) {
            newStyle = { status : "ON",  class3 : "assertive-bg light padding-left padding-right rounded" }
            newStyle1 = {
                descr  : "Emergency Stop heating",
                class2 : "assertive",
                class3 : "button icon ion-checkmark-circled",
                widgetConfig : {
                fill        : "#FF5050",
                fillPressed : "#FF7070",
                disabled: 0
              }
            }
        } else {
            newStyle = { status : "OFF", class3 : "calm-bg light padding-left padding-right rounded" }
            newStyle1 = {
                descr  : "Heating already stopped",
                class2 : "calm",
                class3 : "button icon ion-close-circled",
//                style3 : "display:none;",
                widgetConfig : {
                 fill : "#AAAAAA",
                 fillPressed: "#EEEEEE",
                 disabled: 1
                }
            }
        }
        lastHeater = heater;
        client.publish( config[5].topic + "/status", JSON.stringify(newStyle)  );
        client.publish( config[6].topic + "/status", JSON.stringify(newStyle1) );
  }
}
////////////////////////////////////////////////
// run main
console.log('Start');

setInterval(function() {
   // check current temp and run heater
   if (waterTemp < hiTemp && !heater) {
      if (waterTemp < lowTemp && !manualStop) {
         heater = true;
         pubHeaterStatus();
      }
   }
   if (waterTemp < lowTemp && !manualStop) {
      heater = true;
      pubHeaterStatus();
   }
   if (waterTemp >= hiTemp ) {
      heater = false;
      pubHeaterStatus();
   }
}, 200);

setInterval(function() {
   if (heater && !manualStop) {
      waterTemp = waterTemp + 1;
      console.log("waterTemp inc="+waterTemp);
      pubAlert();
      pubWaterTempStatus();
   }
}, 1000);
setInterval(function() {
   if (!heater || manualStop) {
      waterTemp = waterTemp - 1;
      console.log("waterTemp dec="+waterTemp);
      pubAlert();
      pubWaterTempStatus();
   }
}, 1000);

////////////////////////////////////////////////
