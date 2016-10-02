////////////////////////////////////////////////
//
// Demo device emulator 
//
//
//
// IoT Manager https://play.google.com/store/apps/details?id=ru.esp8266.iotmanager
//
// version     : 1.0
// IoT Manager : 1.5.0 and above
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


var waterTemp = 20;
var hiTemp    = 40;
var hysteresis= 5;

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
  status : "My Home"
};


// Outdoor temp
widget    = "anydata";
id        = "1"
config[1] = {
  id     : id,
  page   : "boiler",
  pageId : 1,
  widget : widget,
  class1 : "item no-border",
  style2 : "font-size:20px;float:left",
  descr  : "Outdoor temp",
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class3 : "balanced",
  style3 : "font-size:40px;font-weight:bold;float:right",
};

// Indoor temp
widget    = "anydata";
id        = "2"
config[2] = {
  id     : id,
  page   : "boiler",
  pageId : 1,
  widget : widget,
  class1 : "item no-border",
  style2 : "font-size:20px;float:left;line-height:3em",
  descr  : "Indoor temp",
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class3 : "balanced-bg light padding-left padding-right rounded",
  style3 : "font-size:40px;font-weight:bold;float:right;line-height:1.5em",
};


// Humidity
widget    = "anydata";
id        = "3"
config[3] = {
  id     : id,
  page   : "boiler",
  pageId : 1,
  widget : widget,
  class1 : "item no-border",
  style2 : "font-size:20px;float:left;line-height:3em",
  descr  : "Humidity",
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class3 : "balanced-bg light padding-left padding-right rounded",
  style3 : "font-size:40px;font-weight:bold;float:right;line-height:1.5em",
};

// CO2
widget    = "anydata";
id        = "4"
config[4] = {
  id     : id,
  page   : "boiler",
  pageId : 1,
  widget : widget,
  class1 : "item no-border",
  style2 : "font-size:30px;float:left",
  descr  : "CO2",
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class3 : "balanced",
  style3 : "font-size:30px;font-weight:bold;float:right",
};



client.on('connect', function () {
  console.log('Broker connected');
  client.subscribe(prefix, { qos : 1 }); // HELLO expected
  // client.subscribe(prefix + "/" + deviceID +"/+/control", { qos : 1 }); // all commands, not used int this example
  pubConfig();
});

client.on('error', function () {
  console.log('Broker error');
});

client.on('offline', function () {
  console.log('Broker offline');
});

client.on('message', function (topic, message) {
  
  console.log("Message arrived: " + topic.toString() + " => " + message.toString());

  if (topic.toString() === prefix && message.toString() == "HELLO" ){
    console.log('HELLO detected');
    pubConfig();
  }
  pubStatus();

})
////////////////////////////////////////////////
function pubConfig() {
    config.forEach(function(item, i, arr) {
      client.publish(prefix + "/" + deviceID + "/config", JSON.stringify(item),{ qos : 1 });
    });    
}
////////////////////////////////////////////////
function pubStatus() {
  var outdoor = 10 + Math.round(Math.random() * 5);
  var indoor = 18 + Math.round(Math.random() * 5);
  var hum = 60 - Math.round(Math.random() * 30);
  var co2 = Math.random();
  var status = {};
  if (co2 > 0.5) {
     status =  {status:'normal', class3: 'balanced'}
  } else {
     status =  {status:'high', class3: 'assertive'}
  }

  client.publish( config[1].topic+"/status", JSON.stringify({ status: outdoor + "°C" }) );
  client.publish( config[2].topic+"/status", JSON.stringify({ status: indoor + "°C" }) );
  client.publish( config[3].topic+"/status", JSON.stringify({ status: hum + "%" }) );
  client.publish( config[4].topic+"/status", JSON.stringify(status) );
  console.log('Publish outdoor:' + outdoor + ' indoor:' + indoor + ' hum:' + hum);
}
////////////////////////////////////////////////
// run main
console.log('Start');

setInterval(function() {
     pubStatus();
}, 5000);
