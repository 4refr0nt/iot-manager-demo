////////////////////////////////////////////////
//
// gauge widget demo
//
// IoT Manager 
// for Android https://play.google.com/store/apps/details?id=ru.esp8266.iotmanager
// for iOS     https://itunes.apple.com/us/app/iot-manager/id1155934877
//
// script version : 1.0
// IoT Manager    : 1.5.5 and above
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


var deviceID = "test-gauge";
var prefix   = "/IoTmanager";
var config   = [];
var client   = mqtt.connect(opt);

var widget   = "gauge";
var id       = "0"
config[0] = {
  id     : id,
  widget : widget,
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  descr  : "Speed",
  class1 : "text-center col-xs-4 no-padding-left no-padding-right",
  widgetConfig : {
    append: 'km/h'
  }
};

id        = "1"
config[1] = {
  id     : id,
  widget : widget,
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  descr  : "Power",
  class1 : "text-center col-xs-4 no-padding-left no-padding-right",
  widgetConfig : {
    type: "semi",
    size: 150,
    append: 'kW',
  }
};

id        = "2"
config[2] = {
  id     : id,
  widget : widget,
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  descr  : "Profit",
  class1 : "text-center col-xs-4 no-padding-left no-padding-right",
  widgetConfig : {
    type: "arch",
    size: 150,
    append: '%',
  }
};

id        = "3"
config[3] = {
  id     : id,
  widget : widget,
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  descr  : "Sales",
  class1 : "text-center col-xs-4 no-padding-left no-padding-right",
  widgetConfig : {
    type   : "full",
    thick  : 8,
    append :"%"
  }
};

id        = "4"
config[4] = {
  id     : id,
  widget : widget,
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  descr  : "Pressure",
  class1 : "text-center col-xs-4 no-padding-left no-padding-right",
  widgetConfig : {
    type   : "arch",
    thick  : 12,
    append :"Pa",
    color  : '#ffaaaa',
    textColor  : '#ff5555',
    backgroundColor: 'rgba(255,255,255, 0.8)'
  }
};

id        = "5"
config[5] = {
  id     : id,
  widget : widget,
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  descr  : "Usage",
  class1 : "text-center col-xs-4 no-padding-left no-padding-right",
  widgetConfig : {
    type   : "arch",
    thick  : 12,
    cap    : "butt",
    append : "Gb",
    color  : '#aaffaa',
    backgroundColor: '#ffaaaa'
  }
};

id        = "6"
config[6] = {
  id     : id,
  widget : widget,
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class1 : "text-center col-xs-6 no-padding-left no-padding-right",
  descr  : "maxValue 150",
  widgetConfig : {
    type   : "arch",
    thick  : 8,
    maximum: 150,
    color  : '#aaaaff',
    backgroundColor: '#fff'
  }
};

id        = "7"
config[7] = {
  id     : id,
  widget : widget,
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class1 : "text-center col-xs-6 no-padding-left no-padding-right",
  descr  : "Prepend",
  widgetConfig : {
    type   : "arch",
    thick  : 8,
    prepend: "$",
    color  : '#55ff55'
  }
};

id        = "8"
config[8] = {
  id     : id,
  widget : widget,
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class1 : "item no-border no-padding text-center",
  descr  : "maxValue 200",
  widgetConfig : {
    type   : "semi",
    size   : 300,
    thick  : 20,
    maximum: 200,
    color  : '#11c1f3',
    backgroundColor: 'rgba(0,0,0, 0.2)'
  }
};

client.on('connect', function () {
  console.log('Broker connected');
  client.subscribe(prefix, { qos : 1 }); // HELLO expected
  pubConfig();
  setInterval(function() {
     pubStatus();
  }, 3000);
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

})
////////////////////////////////////////////////
function pubConfig() {
    config.forEach(function(item, i, arr) {
      client.publish(prefix + "/" + deviceID + "/config", JSON.stringify(item),{ qos : 1 });
    });    
    pubStatus();
}
////////////////////////////////////////////////
function pubStatus() {
  var speed     = 60 + Math.round(Math.random() * 30);
  var power     = 70 + Math.round(Math.random() * 30);
  var profit    = 80 + Math.round(Math.random() * 20);
  var sales     = 90 + Math.round(Math.random() * 10);
  var pressure  = 50 + Math.round(Math.random() * 50);
  var usage     = 40 + Math.round(Math.random() * 60);
  var used      = 100 + Math.round(Math.random() * 50);
  var prepend   = 20 + Math.round(Math.random() * 80);
  var noappend  = 100 + Math.round(Math.random() * 100);

  client.publish( config[0].topic+"/status", JSON.stringify({ status: speed  }) );
  client.publish( config[1].topic+"/status", JSON.stringify({ status: power  }) );
  client.publish( config[2].topic+"/status", JSON.stringify({ status: profit }) );
  client.publish( config[3].topic+"/status", JSON.stringify({ status: sales  }) );
  client.publish( config[4].topic+"/status", JSON.stringify({ status: pressure }) );
  client.publish( config[5].topic+"/status", JSON.stringify({ status: usage    }) );
  client.publish( config[6].topic+"/status", JSON.stringify({ status: used     }) );
  client.publish( config[7].topic+"/status", JSON.stringify({ status: prepend  }) );
  client.publish( config[8].topic+"/status", JSON.stringify({ status: noappend }) );
  console.log('Publish status');
}
////////////////////////////////////////////////
// run main
console.log('Start');

