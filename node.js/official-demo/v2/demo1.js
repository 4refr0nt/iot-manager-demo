////////////////////////////////////////////////
//
// IoT Manager NODE.JS demo client
// version : 1.0.0
//
// Exchange protocol version: 2.0
//
////////////////////////////////////////////////
//
////////////////////////////////////////////////
//
// Connection
//
var host     = 'm11.cloudmqtt.com';
var port     = 10927;
var user     = "test";
var pass     = "test";
var prefix   = "/IoTmanager";
var deviceId = "dev-0001";
////////////////////////////////////////////////

var mqtt = require('mqtt');
var opt = {
    host: host,
    port: port,
    username: user,
    password: pass,
    clientId: 'iotm_client_' + Math.random().toString(16).substr(2, 8),
    protocolId: 'MQTT',
    connectTimeout: 3000
};

var config = [];

var pagesList = {
    pages: [
        { pageId: 10, pageName: "page1" },
        { pageId: 20, pageName: "page2" },
        { pageId: 30, pageName: "page3" },
        { pageId: 40, pageName: "page4" }
    ]
};

// First line
var widget = "anydata";
var id = "0"
config[0] = {
    id: id,
    page: pagesList.pages[0], // page1
    widget: widget,
    class1: "item no-border",
    style2: "font-size:16px;",
    topic: prefix + "/" + deviceId + "/" + widget + id,
    class3: "calm text-center",
    style3: "font-size:20px;",
    status: "My Home"
};

// Outdoor temp
widget = "anydata";
id = "1"
config[1] = {
    id: id,
    page: pagesList.pages[1], // page2
    pageId: 10,
    widget: widget,
    class1: "item no-border",
    style2: "font-size:20px;float:left",
    descr: "Outdoor temp",
    topic: prefix + "/" + deviceId + "/" + widget + id,
    class3: "assertive",
    style3: "font-size:40px;font-weight:bold;float:right",
};

// Indoor temp
widget = "anydata";
id = "2"
config[2] = {
    id: id,
    page: pagesList.pages[1], // page2
    pageId: 20,
    widget: widget,
    class1: "item no-border",
    style2: "font-size:20px;float:left;line-height:3em",
    descr: "Indoor temp",
    topic: prefix + "/" + deviceId + "/" + widget + id,
    class3: "balanced-bg light padding-left padding-right rounded",
    style3: "font-size:40px;font-weight:bold;float:right;line-height:1.5em",
};


// Humidity
widget = "anydata";
id = "3"
config[3] = {
    id: id,
    page: pagesList.pages[2], // page3
    widget: widget,
    class1: "item no-border",
    style2: "font-size:20px;float:left;line-height:3em",
    descr: "Humidity",
    topic: prefix + "/" + deviceId + "/" + widget + id,
    class3: "balanced-bg light padding-left padding-right rounded",
    style3: "font-size:40px;font-weight:bold;float:right;line-height:1.5em",
};

// CO2
widget = "anydata";
id = "4"
config[4] = {
    id: id,
    page: pagesList.pages[3], // page4
    widget: widget,
    class1: "item no-border",
    style2: "font-size:30px;float:left",
    descr: "CO2",
    topic: prefix + "/" + deviceId + "/" + widget + id,
    class3: "balanced",
    style3: "font-size:30px;font-weight:bold;float:right",
};

var client = mqtt.connect(opt);


client.on('connect', function() {
    console.log('mqtt: Broker connected');

    //client.subscribe(prefix, { qos : 1 }); // HELLO expected (only for old devices)
    client.subscribe(prefix + "/+/request", { qos: 1 }); // requests from all mobile devices

    // client.subscribe(prefix + "/" + deviceId + "/+/control", { qos: 1 }); // all control commands for this device - compatible with old exchange protocol, but not used in this example
    // pubConfig(); // I'm online now! - old style
    pubPages();     // I'm online now! - new style
});


client.on('error', function() {
    console.log('mqtt: Error');
});


client.on('offline', function() {
    console.log('mqtt: Offline');
});


client.on('message', function(topic, message) {

    console.log("\nMessage arrived:  msg topic:" + topic.toString() + " => " + message.toString());

    if (topic.toString() === prefix && message.toString() == "HELLO") {

        // this code never executed, because not subscribing to topic prefix - "/IoTmanager"
        console.log('HELLO detected');
        // pubConfig(); /////////// old exchange protocol - actions not needed for exchange protocol v2
    }

    // get mobile device id - may be useful for different response to different mobile device, not used in this example
    var id = topic.toString().split("/")[2]; 

    // new exchange protocol?
    if (topic.toString() === prefix + "/" + id + "/request") { 

        // extract command and param
        var msg = JSON.parse(message.toString()); 

        console.log('New request from IoT Manager: id="' + id + '", command="' + msg.command + '", param="' + msg.param + '"');

        // response: my pages list
        if (msg.command === "getPages") { 

            console.log("Command detected: getPages");
            pubPages();

        // response: widgets config from one page only
        } else if (msg.command === "getPageById") { 

            console.log('Command detected: "getPageById", param: "' + msg.param + '"');

            pagesList.pages.forEach(function(item, i, arr) {

                // it is one of our pages?
                if (item.pageId === msg.param) { 
                    console.log('Request is for existing page "' + item.pageName + '", pageId="' + msg.param + '"');
                    pubPage(msg.param);
                }
            });

        }

    }
})


function pubPage(page) {

    // check all widgets and pub widgets config from requested page
    config.forEach(function(item, i, arr) {

        if (item.page.pageId == page) {

            // pub config for one widget
            console.log('Action: response config widget id="' + item.id + '"');
            client.publish(prefix + "/" + deviceId + "/config", JSON.stringify(item), { qos: 1 });

            // pub status for one widget
            pubStatus(i);
        }
    });
}


function pubStatus(i) {
    if (i == 0) {
        // do nothing - in this example widget 0 without status
        return;

    } else if (i == 1) {

        status = 10 + Math.round(Math.random() * 5) + "°C";
        console.log('Action: response status outdoor:' + status);

    } else if (i == 2) {

        status = 18 + Math.round(Math.random() * 5) + "°C";
        console.log('Action: response status indoor:' + status);

    } else if (i == 3) {
        status = 50 + Math.round(Math.random() * 20) + "%";
        console.log('Action: response status hum:' + status);

    } else if (i == 4) {

        status = "normal";
        console.log('Action: response status CO2:' + status);
    }

    client.publish(config[i].topic + "/status", JSON.stringify({ status: status }));

}


function pubPages() {

    console.log('Action: Pub pages list');
    client.publish(prefix + "/" + deviceId + "/response", JSON.stringify(pagesList));
}

// run main
console.log('Start');
