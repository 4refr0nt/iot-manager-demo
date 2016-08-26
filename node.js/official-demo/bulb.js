////////////////////////////////////////////////
//
// Demo device emulator for "steel" widget (bulb)
//
//
// IoT Manager https://play.google.com/store/apps/details?id=ru.esp8266.iotmanager
//
// version     : 1.0
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
  id     :"1",
  descr  :"bulb 1",
  widget :"steel",
  topic  :prefix + "/" + deviceID + "/bulb1",
  class1 : "col-xs-4 text-center",
  widgetConfig : {
     width  : "auto2",
     height : 200,
     type   : "LightBulb",
     color  : "rgb(204, 51, 0)", // red
  }
};

var config2 = {
  id     :"2",
  descr  :"bulb 2",
  widget :"steel",
  topic  :prefix + "/" + deviceID + "/bulb2",
  class1 : "col-xs-4 text-center",
  widgetConfig : {
     width  : "auto2",
     height : 200,
     type   : "LightBulb",
     color  : "rgb(0, 255, 0)", // green
  }
};

var config3 = {
  id     :"3",
  descr  :"battery",
  widget :"steel",
  topic  :prefix + "/" + deviceID + "/battery1",
  class1 : "item text-center no-border",
  widgetConfig : {
     width  : 80,
     height : 36,
     type   : "Battery"
  }
};

var config4 = {
  id     :"4",
  descr  :"led",
  widget :"steel",
  topic  :prefix + "/" + deviceID + "/led4",
  class1 : "col-xs-3 text-center",
  widgetConfig : {
     width  : 50,
     height : 50,
     type   : "Led",
     blink  : false,
     LedColor: "GREEN_LED", // RED_LED GREEN_LED BLUE_LED ORANGE_LED YELLOW_LED CYAN_LED MAGENTA_LED
  }
};

var config5 = {
  id     :"5",
  descr  :"led",
  widget :"steel",
  topic  :prefix + "/" + deviceID + "/led5",
  class1 : "col-xs-6 text-center",
  widgetConfig : {
     width  : 100,
     height : 100,
     type   : "Led",
     blink  : true,
     LedColor: "CYAN_LED", // RED_LED GREEN_LED BLUE_LED ORANGE_LED YELLOW_LED CYAN_LED MAGENTA_LED
  }
};

var config6 = {
  id     :"6",
  descr  :"led",
  widget :"steel",
  topic  :prefix + "/" + deviceID + "/led6",
  class1 : "col-xs-2 text-center",
  widgetConfig : {
     width  : 50,
     height : 50,
     type   : "Led",
     blink  : true,
     LedColor: "RED_LED", // RED_LED GREEN_LED BLUE_LED ORANGE_LED YELLOW_LED CYAN_LED MAGENTA_LED
  }
};


var config7 = {
  id     :"7",
  descr  :"odometer",
  widget :"steel",
  topic  :prefix + "/" + deviceID + "/odo",
  class1 : "text-center",
  widgetConfig : {
     width  : 100,
     height : 40,
     type   : "Odometer",
  }
};

var bulb1alpha  = 0;
var bulb2alpha  = 0;
var batVal = 0;
var batInc = 5;
var led4 = 0;
var odo  = 123.0;

var client  = mqtt.connect(opt);

client.on('connect', function () {
  console.log('Broker connected');
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
  }
});

console.log('Start');
    setInterval(function() {
      bulb1alpha = Math.random() * 100;
      bulb2alpha = Math.random() * 100;
      led4 = Math.round(Math.random() * 1);
      pubStatus1();
      battery();

    }, 2000);

    setInterval(function() {
      updateOdo();

    }, 200);
////////////////////////////////////////////////
function pubConfig() {
    client.publish(prefix + "/" + deviceID + '/config', JSON.stringify(config1));
    client.publish(prefix + "/" + deviceID + '/config', JSON.stringify(config2));
    client.publish(prefix + "/" + deviceID + '/config', JSON.stringify(config3));
    client.publish(prefix + "/" + deviceID + '/config', JSON.stringify(config4));
    client.publish(prefix + "/" + deviceID + '/config', JSON.stringify(config5));
    client.publish(prefix + "/" + deviceID + '/config', JSON.stringify(config6));
    client.publish(prefix + "/" + deviceID + '/config', JSON.stringify(config7));
    console.log('pubConfig');
}
////////////////////////////////////////////////

function pubStatus1() {
      console.log("Send status");
      client.publish(config1.topic + '/status', JSON.stringify({ status: bulb1alpha }));
      client.publish(config2.topic + '/status', JSON.stringify({ status: bulb2alpha }));
      client.publish(config3.topic + '/status', JSON.stringify({ status: batVal }));
      client.publish(config4.topic + '/status', JSON.stringify({ status: led4 }));
      client.publish(config7.topic + '/status', JSON.stringify({ status: odo }));
}
////////////////////////////////////////////////
    function battery() {
        batVal += batInc;
        if (batVal < 0 || batVal > 100) {batInc = -batInc;}
    }
    function updateOdo() {
        odo += 0.005
    }
