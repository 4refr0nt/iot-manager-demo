////////////////////////////////////////////////
// User defined section
var host = '192.168.122.1';
var port = 1883;
var user = "";
var pass = "";
 
var IoTmanager = {
    topic: "/IoTmanager",
    cachefile: "/opt/test/iotcache" //куда писать кэш, файл должен существовать, в него лучше записать что-то вроде  { "one":"one"}
}
 
////////////////////////////////////////////////
 
var opt = {
  host       : host,
  port       : port,
  username   : user,
  password   : pass,
  clientId   : 'mqttjs_' + Math.random().toString(16).substr(2, 8),
  protocolId : 'MQTT',
  connectTimeout: 3000
};
 
 
 
mqtt = require('mqtt');
fs = require('fs');
 
run();
function run() {
    mqttclient = mqtt.connect(opt);
    setTimeout(function() {
    cachefile = fs.readFileSync(IoTmanager.cachefile.toString());
    states = JSON.parse(cachefile);
    console.log("Cache ready");
    }, 100);
    setTimeout(function() {
            fs.writeFile(IoTmanager.cachefile.toString(), JSON.stringify(states));
    }, 5000);
 
    mqttclient.on('connect', function() {
            console.log("Connected to MQTT server at "+opt.host+":"+opt.port);
            mqttclient.subscribe(IoTmanager.topic);
            console.log("subscribed to "+IoTmanager.topic);
            mqttclient.publish(IoTmanager.topic, 'HELLO');
            });
 
        mqttclient.on('close', function() {
        fs.writeFile(IoTmanager.cachefile.toString(), JSON.stringify(states));
        console.log('mqtt closed');          
            });
        mqttclient.on('message', function(topic, message) {
            if (topic.toString() == IoTmanager.topic && message.toString() == "HELLO" ){
            } else if (topic.split("/").length == 1) {
                mqttclient.subscribe(IoTmanager.topic+"/"+message.toString()+"/config");
                } else if (topic.split("/").length == 2 ) {
                mqttclient.subscribe(IoTmanager.topic+"/"+message.toString()+"/config");
                if ( typeof states !== 'undefined' ) {
                if ( states[message.toString()+"_stored"] !== 'undefined' ) {
                    Object.keys(states).forEach(function(device) {
                        if ( device.split("_")[0] == message.toString() && device.toString() !== states[message.toString()+"_stored"] ) {
                            setTimeout(function() {
                            mqttclient.publish(IoTmanager.topic.toString()+"/"+message.toString()+"/"+device.split("_")[1]+"/control", states[device].toString());
                            }, 500);
                        }
                    });                  
                }
                }
                if ( typeof states !== 'undefined' ) { fs.writeFile(IoTmanager.cachefile.toString(),JSON.stringify(states));}
            } else if (topic.split("/")[3] == 'config' ) {
                mqttclient.subscribe(JSON.parse(message.toString()).topic+'/status');
                mqttclient.subscribe(JSON.parse(message.toString()).topic+'/control');
            } else if (topic.split("/").length == 4 ) {
            } else if (topic.split("/")[4] == 'status' ) {
                states[topic.split("/")[2].toString()+"_stored"] = "";
                states[topic.split("/")[2].toString()+"_"+topic.split("/")[3].toString()] = JSON.parse(message.toString()).status;
            }
    });
};
