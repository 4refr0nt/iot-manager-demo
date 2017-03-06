////////////////////////////////////////////////
//
// Clear all messages with RETAIN flag
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

var client  = mqtt.connect(opt);

client.on('connect', function () {
  console.log('Broker connected');
  client.subscribe('#');
});

client.on('error', function () {
  console.log('error');
});

client.on('offline', function () {
  console.log('offline');
});

client.on('message', function (topic, message) {
  if (message.toString().length > 0) {
    console.log("receive '" + topic + "' : '" + message.toString() + "'")
//    client.publish(topic, null, {retain: true});
    console.log('clear topic ' + topic)
  }
});

