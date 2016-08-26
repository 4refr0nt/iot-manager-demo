////////////////////////////////////////////////
//
// Demo device emulator for "steel" widget (linear)
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
  descr  :"Steel 1",
  widget :"steel",
  topic  :prefix + "/" + deviceID + "/steel1",
  widgetConfig : {
     width  : "auto",
     height : 100,
     type   : "Linear",
     titleString: "Thermometer 1",
     unitString: "temp C",
     threshold: 90
  }
};

var config2 = {
  id     :"2",
  descr  : "Steel 2",
  widget : "steel",
  topic  : prefix + "/" + deviceID + "/steel2",
  widgetConfig : {
     width       : "auto",
     height      : 140,
     type        : "Linear",
     gaugeType   : "TYPE2",
     titleString : "Thermometer 2",
     unitString  : "Unit",
     threshold   : 70,
     lcdVisible  : true,
     lcdDecimals : 0,
     // optional parameters
     FrameDesign : "STEEL",       // BLACK_METAL METAL SHINY_METAL BRASS STEEL CHROME GOLD ANTHRACITE TILTED_GRAY TILTED_BLACK GLOSSY_METAL
     BackgroundColor: "CARBON",   // DARK_GRAY SATIN_GRAY LIGHT_GRAY WHITE BLACK BEIGE BROWN RED GREEN BLUE ANTHRACITE MUD PUNCHED_SHEET CARBON STAINLESS BRUSHED_METAL BRUSHED_STAINLESS TURNED
     ColorDef    : "RAITH",    // RED GREEN BLUE ORANGE YELLOW CYAN MAGENTA WHITE GRAY BLACK RAITH GREEN_LCD JUG_GREEN
     LcdColor    : "RED_DARKRED",       // BEIGE BLUE ORANGE RED YELLOW WHITE GRAY BLACK GREEN BLUE2 BLUE_BLACK BLUE_DARKBLUE BLUE_GRAY STANDARD STANDARD_GREEN BLUE_BLUE RED_DARKRED DARKBLUE LILA BLACKRED DARKGREEN AMBER LIGHTBLUE SECTIONS
     LedColor    : "MAGENTA_LED", // RED_LED GREEN_LED BLUE_LED ORANGE_LED YELLOW_LED CYAN_LED MAGENTA_LED
  }
};

var config3 = {
  id     :"3",
  descr  : "Steel 3",
  widget : "steel",
  topic  : prefix + "/" + deviceID + "/steel3",
  widgetConfig : {
     width       : "auto",
     height      : 140,
     type        : "LinearBargraph",
     titleString : "Gradient Colors",
     unitString  : "Unit",
     threshold   : 30,
     useValueGradient: true,

     // optional parameters
     FrameDesign : "CHROME",        // BLACK_METAL METAL SHINY_METAL BRASS STEEL CHROME GOLD ANTHRACITE TILTED_GRAY TILTED_BLACK GLOSSY_METAL
     BackgroundColor: "TURNED",    // DARK_GRAY SATIN_GRAY LIGHT_GRAY WHITE BLACK BEIGE BROWN RED GREEN BLUE ANTHRACITE MUD PUNCHED_SHEET CARBON STAINLESS BRUSHED_METAL BRUSHED_STAINLESS TURNED
     ColorDef    : "ORANGE",      // RED GREEN BLUE ORANGE YELLOW CYAN MAGENTA WHITE GRAY BLACK RAITH GREEN_LCD JUG_GREEN
     LcdColor    : "BLUE_BLUE", // BEIGE BLUE ORANGE RED YELLOW WHITE GRAY BLACK GREEN BLUE2 BLUE_BLACK BLUE_DARKBLUE BLUE_GRAY STANDARD STANDARD_GREEN BLUE_BLUE RED_DARKRED DARKBLUE LILA BLACKRED DARKGREEN AMBER LIGHTBLUE SECTIONS
     LedColor    : "ORANGE_LED",  // RED_LED GREEN_LED BLUE_LED ORANGE_LED YELLOW_LED CYAN_LED MAGENTA_LED
  }
};

var config4 = {
  id     :"4",
  descr  : "Steel 4",
  widget : "steel",
  topic  : prefix + "/" + deviceID + "/steel4",
  style1 : "float:left;",
  widgetConfig : {
     width       : "auto2",
     height      : 100,
     type        : "Linear",
     frameVisible: false,
     minMeasuredValueVisible : true,
     maxMeasuredValueVisible : true,
     minValue : -50,
     maxValue :  50,
     thresholdVisible : false
  }
};

var config5 = {
  id     :"5",
  descr  : "Steel 5",
  widget : "steel",
  topic  : prefix + "/" + deviceID + "/steel5",
  style1 : "float:right;",
  widgetConfig : {
     width       : "auto2",
     height      : 100,
     type        : "Linear",
     frameVisible: false
  }
};

var temp  = 0;
var temp2 = 0;
var temp3 = 0;
var temp4 = 0;
var temp5 = 0;

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
      temp  = Math.random() * 100;
      temp2 = Math.random() * 100;
      temp3 = Math.random() * 100;
      temp4 = Math.random() * 50;
      temp5 = Math.random() * 50;
      pubStatus1();

    }, 5000);
////////////////////////////////////////////////
function pubConfig() {
    client.publish(prefix + "/" + deviceID + '/config', JSON.stringify(config1));
    client.publish(prefix + "/" + deviceID + '/config', JSON.stringify(config2));
    client.publish(prefix + "/" + deviceID + '/config', JSON.stringify(config3));
    client.publish(prefix + "/" + deviceID + '/config', JSON.stringify(config4));
    client.publish(prefix + "/" + deviceID + '/config', JSON.stringify(config5));
    console.log('pubConfig');
}
////////////////////////////////////////////////

function pubStatus1() {
      console.log("Send status");
      client.publish(config1.topic + '/status', JSON.stringify({ status: temp  }));
      client.publish(config2.topic + '/status', JSON.stringify({ status: temp2 }));
      client.publish(config3.topic + '/status', JSON.stringify({ status: temp3 }));
      client.publish(config4.topic + '/status', JSON.stringify({ status: temp4 }));
      client.publish(config5.topic + '/status', JSON.stringify({ status: temp5 }));
}
