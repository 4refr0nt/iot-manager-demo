////////////////////////////////////////////////
//
// Demo device emulator for char widget
//
//
//
// IoT Manager https://play.google.com/store/apps/details?id=ru.esp8266.iotmanager
//
// version     : 1.0
// IoT Manager : 1.5.3 and above
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


var deviceID = "chart";
var prefix   = "/IoTmanager";
var config   = [];
var client   = mqtt.connect(opt);

// First line
var widget   = "anydata";
var id       = "0"
config[0] = {
  id     : id,
  widget : widget,
  class1 : "item no-border",
  style2 : "font-size:16px;",
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class3 : "calm text-center",
  style3 : "font-size:20px;",
  status : "My Home"
};


widget    = "chart";
id        = "1"
config[1] = {
  id     : id,
  widget : widget,
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  widgetConfig: {
      type: 'line', // line, bar, horizontalBar, radar, polarArea, doughnut, pie, bubble
      maxCount: 20
  }
};

widget    = "chart";
id        = "2"
config[2] = {
  id     : id,
  widget : widget,
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  widgetConfig: {
      type: 'line', // line, bar, horizontalBar, radar, polarArea, doughnut, pie, bubble
      maxCount: 10
  }
};

widget    = "chart";
id        = "3"
config[3] = {
  id     : id,
  widget : widget,
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  widgetConfig: {
      type: 'bar', // line, bar, horizontalBar, radar, polarArea, doughnut, pie, bubble
      maxCount: 5
  }
};

client.on('connect', function () {
  console.log('Broker connected');
  client.subscribe(prefix, { qos : 1 }); // HELLO expected
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

  var x = Math.round(Math.random() * 5 - 5);

  var y = Math.round(Math.random() * 10 - 5);

  var z = Math.round(Math.random() * 15 - 10);

  client.publish( config[1].topic+"/status", JSON.stringify({ status: x }) );
  client.publish( config[2].topic+"/status", JSON.stringify({ status: [x, y ] }) );
  client.publish( config[3].topic+"/status", JSON.stringify({ status: [x, y, z] }) );

  console.log('Publish');
}
////////////////////////////////////////////////
// run main
console.log('Start');

  setInterval(function() {
     pubStatus();
  }, 1000);
