////////////////////////////////////////////////
//
// Markdown to HTML converter demo (markdown widget)
//
//
// IoT Manager https://play.google.com/store/apps/details?id=ru.esp8266.iotmanager
//
// version     : 1.0
// IoT Manager : 1.5.4 and above
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


var deviceID = "device10";
var prefix   = "/IoTmanager";
var config   = [];
var client   = mqtt.connect(opt);

// First line
var widget   = "anydata";
var id       = "0"

widget    = "markdown";
id        = "0"
config[0] = {
  id     : id,
  widget : widget,
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class1 : "padding"
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

  var x = 10 + Math.round(Math.random() * 10);
  var y = 100 + Math.round(Math.random() * 100);
  var z = 1000 + Math.round(Math.random() * 1000);
  var z1= 1000 + Math.round(Math.random() * 5000);
  var text  = '\n';
      text += '\n__Sensor1 ' + x + '__';
      text += '\n~~Sensor2 ' + y + '~~';
      text += '\n**Sensor3 ' + z + '**';
      text += '\n*Sensor4  ' + z1+ '*';
      text += '\n';
      text += '\n| Tables   | Are           | Cool  |';
      text += '\n| -------- |:-------------:|------:|';
      text += '\n| col 2    | centered      |'+x+'|';
      text += '\n| col 3    | **right**     |'+y+'|';
      text += '\n| col 1    | ~~left~~      |'+z+'|';
      text += '\n|          |     4         |'+z1+'|';
      text += '\n';
      text += '\n# Sensor 1: '+x;
      text += '\n## Sensor 2: '+y;
      text += '\n### Sensor 3: '+z;
      text += '\n#### Sensor 4: '+z1;
      text += '\n';
  client.publish( config[0].topic+"/status", JSON.stringify({ status: text }) );

  console.log('Publish');
}
////////////////////////////////////////////////
// run main
console.log('Start');

  setInterval(function() {
     pubStatus();
  }, 5000);
