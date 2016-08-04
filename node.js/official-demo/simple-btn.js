////////////////////////////////////////////////
//
// Demo device emulator for "simple-btn" widget
//
//
//
// IoT Manager https://play.google.com/store/apps/details?id=ru.esp8266.iotmanager
//
// version     : 1.0
// IoT Manager : 1.4.7 and above
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

var deviceID = "uniqueId-0002";
var prefix   = "/IoTmanager";
var config   = [];
var widget   = "simple-btn";

var id       = "0"
config[0] = {
  id     : id,
  page   : "b1",
  pageId : 1,
  widget : widget,
  class1 : "item no-border no-padding-bottom",
  descr  : "Fab button with any colors",
  class2 : "balanced padding-top",
  style2 : "float:left;font-size:16px;",
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class3 : "button button-fab icon ion-bug",
  style3 : "float:right;",
  widgetConfig : {
    fill          : "#724200",     // You can use any HTML colors
    fillPressed   : "#004333",
    label         : "#4990E2",
    labelPressed  : "#FF0000"
  }
};

id = "1"
config[1] = {
  id     : id,
  page   : "b1",
  pageId : 1,
  widget : widget,
  class1 : "item no-border no-padding-bottom",
  descr  : "Another fab button",
  class2 : "assertive padding-top",
  style2 : "float:left;font-weight:bold;",
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class3 : "button button-assertive button-fab icon ion-heart",
  style3 : "float:right;"
};

id = "2"
config[2] = {
  id     : id,
  page   : "b2",
  pageId : 2,
  widget : widget,
  class1 : "col-xs-2 text-center",
  descr  : "Left",
  class2 : "calm",
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class3 : "button button-calm button-fab icon ion-arrow-left-a"
};

id = "3"
config[3] = {
  id     : id,
  page   : "b2",
  pageId : 2,
  widget : widget,
  class1 : "col-xs-3 text-center",
  descr  : "Right",
  class2 : "calm",
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class3 : "button button-calm button-fab icon ion-arrow-right-a"
};

id = "4"
config[4] = {
  id     : id,
  page   : "b2",
  pageId : 2,
  widget : widget,
  class1 : "col-xs-3 text-center",
  descr  : "Up",
  class2 : "balanced",
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class3 : "button button-balanced button-fab icon ion-arrow-up-c"
};

id = "5"
config[5] = {
  id     : id,
  page   : "b2",
  pageId : 2,
  widget : widget,
  class1 : "text-center padding-right",
  style1 : "float:right;",
  descr  : "Down",
  class2 : "balanced",
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class3 : "button button-balanced button-fab icon ion-arrow-down-c"
};

id = "6"
config[6] = {
  id     : id,
  page   : "b3",
  pageId : 3,
  widget : widget,
  class1 : "text-center padding-left padding-right",
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class3 : "button button-energized button-block icon-left ion-arrow-shrink",
  style3 : "",
  widgetConfig : {
    title         : "button-block"
  }
};

id = "7"
config[7] = {
  id     : id,
  page   : "b3",
  pageId : 3,
  widget : widget,
  class1 : "col-xs-3",
  style1 : "",
  descr  : "",
  class2 : "",
  style2 : "",
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class3 : "button button-royal button-small",
  style3 : "",
  widgetConfig : {
    title : "small"
  }
};

id = "8"
config[8] = {
  id     : id,
  page   : "b3",
  pageId : 3,
  widget : widget,
  class1 : "col-xs-3 col-xs-offset-1",
  style1 : "",
  descr  : "",
  class2 : "",
  style2 : "",
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class3 : "button button-positive",
  style3 : "",
  widgetConfig : {
    title : "button"
  }
};

id = "9"
config[9] = {
  id     : id,
  page   : "b3",
  pageId : 3,
  widget : widget,
  class1 : "col-xs-3 col-xs-offset-1",
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class3 : "button button-large button-balanced",
  style3 : "",
  widgetConfig : {
    title : "large"
  }
};

id = "10"
config[10] = {
  id     : id,
  page   : "b3",
  pageId : 3,
  widget : widget,
  class1 : "padding-left",
  style1 : "float:left;",
  descr  : "",
  class2 : "",
  style2 : "",
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class3 : "button button-calm button-outline",
  style3 : "",
  widgetConfig : {
    title : "outline"
  }
};

id = "11"
config[11] = {
  id     : id,
  page   : "b3",
  pageId : 3,
  widget : widget,
  style1 : "float:right;",
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class3 : "button button-light button-clear icon-right ion-toggle",
  style3 : "",
  widgetConfig : {
    title : "clear"
  }
};

id = "12"
config[12] = {
  id     : id,
  page   : "b4",
  pageId : 4,
  widget : widget,
  class1 : "item no-border padding-top",
  descr  : "Try long-touch",
  class2 : "energized padding-top",
  style2 : "float:left;font-size:20px;",
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class3 : "button button-energized icon ion-alert-circled",
  style3 : "float:right;",
  widgetConfig : {
    delay  : 200
  }
};

id = "13"
config[13] = {
  id     : id,
  page   : "b4",
  pageId : 4,
  widget : widget,
  class1 : "text-center",
  topic  : prefix + "/" + deviceID + "/" + widget + id,
  class3 : "button button-calm button-block icon-right ion-earth",
  style3 : "",
  widgetConfig : {
    title : "Send custom data",
    data  : "any usefull data"
  }
};


var client  = mqtt.connect(opt);

client.on('connect', function () {
  console.log('Broker connected');
  client.subscribe(prefix, { qos : 1 }); // only HELLO expected
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
  } else {
    console.log("msg: " + topic.toString() + " => " + message.toString());
  }
});

console.log('Start');
pubConfig();

////////////////////////////////////////////////
function pubConfig() {
    client.publish( prefix, deviceID );
    config.forEach(function(item, i, arr) {
      client.publish(prefix + "/" + deviceID + "/config", JSON.stringify(item),{ qos : 1 });
    });    
}
////////////////////////////////////////////////
