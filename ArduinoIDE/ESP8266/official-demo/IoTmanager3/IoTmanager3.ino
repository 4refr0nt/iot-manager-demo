/*
 IoT Manager mqtt device client https://play.google.com/store/apps/details?id=ru.esp8266.iotmanager
 Based on Basic MQTT example with Authentication
 PubSubClient library v 1.91.1 https://github.com/Imroy/pubsubclient
  - connects to an MQTT server, providing userdescr and password
  - publishes config to the topic "/IoTmanager/config/deviceID/"
  - subscribes to the topic "/IoTmanager/hello" ("hello" messages from mobile device) 

  Tested with Arduino IDE 1.6.7 + ESP8266 Community Edition v 2.1.0-rc1 and PubSubClient library v 1.91.1 https://github.com/Imroy/pubsubclient
  ESP8266 Community Edition v 2.1.0-rc1 have some HTTPS issues. Push notification temporary disabled.
  ArduinoJson library 5.0.7 https://github.com/bblanchon/ArduinoJson

  Used D3 Liquid Fill Gauge - brattonc’s block http://bl.ocks.org/brattonc/5e5ce9beee483220e2f6
  
  sketch version : 1.6
  IoT Manager    : 1.4.5 and above

  fillgauge widget demo

  <div class="class1" style="style1">              - class1 and style1 for 1 div (all widget)
    <div class="class2" style="style2">descr</div> - class2 and style2 for 2 div (description)
    <div class="class3" style="style3">            - class3 and style3 for 3 div (svg)
       <svg width="width" height="height"></svg>          
    </div>
  </div>

*/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char *ssid =  "IoT";            // cannot be longer than 32 characters!
const char *pass =  "12345678";       // WiFi password

String prefix   = "/IoTmanager";      // global prefix for all topics - must be some as mobile device
String deviceID = "dev03";            // thing ID - unique device id in our project

WiFiClient wclient;

// config for cloud mqtt broker by DNS hostname ( for example, cloudmqtt.com use: m20.cloudmqtt.com - EU, m11.cloudmqtt.com - USA )
String mqttServerName = "m11.cloudmqtt.com";            // for cloud broker - by hostname, from CloudMQTT account data
int    mqttport = 10927;                                // default 1883, but CloudMQTT.com use other, for example: 13191, 23191 (SSL), 33191 (WebSockets) - use from CloudMQTT account data
String mqttuser =  "test";                              // from CloudMQTT account data
String mqttpass =  "test";                              // from CloudMQTT account data
PubSubClient client(wclient, mqttServerName, mqttport); // for cloud broker - by hostname


// config for local mqtt broker by IP address
//IPAddress server(192, 168, 1, 100);                        // for local broker - by address
//int    mqttport = 1883;                                    // default 1883
//String mqttuser =  "";                                     // from broker config
//String mqttpass =  "";                                     // from broker config
//PubSubClient client(wclient, server, mqttport);            // for local broker - by address

String val;
String ids = "";
int newValue, newtime, oldtime, freeheap;

const int nWidgets = 1;
String sTopic      [nWidgets];
String stat        [nWidgets];
int    pin         [nWidgets];
String thing_config[nWidgets];
StaticJsonBuffer<1024> jsonBuffer;
JsonObject& json_status = jsonBuffer.createObject();
String string_status;

void FreeHEAP() {
  if ( ESP.getFreeHeap() < freeheap ) {
    if ( ( freeheap != 100000) ) {
       Serial.print("Memory leak detected! old free heap = ");
       Serial.print(freeheap);
       Serial.print(", new value = ");
       Serial.println(ESP.getFreeHeap());
    }
    freeheap = ESP.getFreeHeap();
  }
}

String setStatus ( String s ) {
  json_status["status"] = s;
  string_status = "";
  json_status.printTo(string_status);
  return string_status;
}
String setStatus ( int s ) {
  json_status["status"] = s;
  string_status = "";
  json_status.printTo(string_status);
  return string_status;
}
void initVar() {

  pin   [0] = A0;     // ADC
  sTopic[0] = prefix + "/" + deviceID + "/ADC";
  stat  [0] = setStatus (0);

  JsonObject& root = jsonBuffer.createObject();
  JsonObject& cfg  = jsonBuffer.createObject();

  root["id"] = 0;
  root["page"] = "ADC";
  root["widget"] = "fillgauge";
  root["class1"] = "no-border text-center";                               // class for 1st div
  root["style1"] = "";                                        // style for 1st div
  root["descr"] = "Fuel level";                               // text  for description
  root["class2"] = "assertive text-center";                   // class for description from http://ionicframework.com/docs/components/#colors
  root["style2"] = "font-size:20px;font-weight:bold;padding-bottom:10px;padding-top:10px;"; // style for description
  root["topic"] = sTopic[0];
  root["width"] = "150px";                                    // SVG width
  root["height"] = "150px";                                   // SVG height
  root["class3"] = "text-center";                             // class for 3 div - SVG
  root["style3"] = "";                                        // style for 3 div - SVG

  // fillgauge SVG config details see http://bl.ocks.org/brattonc/5e5ce9beee483220e2f6
  cfg["circleThickness"]  = 0.15;
  cfg["circleColor"]      = "#FF7777";
  cfg["textColor"]        = "#FF4444";
  cfg["waveTextColor"]    = "#FFAAAA";
  cfg["waveColor"]        = "#FFDDDD";
  cfg["circleThickness"]  = 0.2;
  cfg["textVertPosition"] = 0.2;
  cfg["waveAnimateTime"]  = 1000;
  cfg["waveHeight"]       = 0.05;
  cfg["waveAnimate"]      = true;
  cfg["waveRise"]         = false;
  cfg["waveHeightScaling"]= false;
  cfg["waveOffset"]       = 0.25;
  cfg["textSize"]         = 0.8;
  cfg["displayPercent"]   = false;
  cfg["minValue"]         = 0;
  cfg["maxValue"]         = 700;
  cfg["waveCount"]        = 3;

  root["widgetConfig"] = cfg;
  root.printTo(thing_config[0]);
}
void pubStatus(String t, String payload) {  
    if (client.publish(t + "/status", payload)) { 
       Serial.println("Publish new status for " + t + ", value: " + payload);
    } else {
       Serial.println("Publish new status for " + t + " FAIL!");
    }
    FreeHEAP();
}
void pubConfig() {
  bool success;
  success = client.publish(MQTT::Publish(prefix, deviceID).set_qos(1));
  if (success) {
      delay(500);
      for (int i = 0; i < nWidgets; i = i + 1) {
        success = client.publish(MQTT::Publish(prefix + "/" + deviceID + "/config", thing_config[i]).set_qos(1));
        if (success) {
          Serial.println("Publish config: Success (" + thing_config[i] + ")");
        } else {
          Serial.println("Publish config FAIL! ("    + thing_config[i] + ")");
        }
        delay(150);
      }      
  }
  if (success) {
     Serial.println("Publish config: Success");
  } else {
     Serial.println("Publish config: FAIL");
  }

  stat[0] = setStatus( analogRead(pin[0]) );
  
  for (int i = 0; i < nWidgets; i = i + 1) {
      pubStatus(sTopic[i], stat[i]);
      delay(100);
  }      
}
void callback(const MQTT::Publish& sub) {
  Serial.print("Get data from subscribed topic ");
  Serial.print(sub.topic());
  Serial.print(" => ");
  Serial.println(sub.payload_string());

  if ( sub.payload_string() == "HELLO" ) {  // handshaking
     pubConfig();
  }
}

void setup() {
  WiFi.mode(WIFI_STA);
  initVar();
  oldtime = 0;
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.println("MQTT client started.");
  FreeHEAP();
  freeheap = 100000;
  WiFi.disconnect();
  WiFi.printDiag(Serial);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting via WiFi to ");
    Serial.print(ssid);
    Serial.println("...");

    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      return;
    }

    Serial.println("");
    Serial.println("WiFi connect: Success");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (!client.connected()) {
      Serial.println("Connecting to MQTT server ...");
      bool success;
      if (mqttuser.length() > 0) {
        success = client.connect( MQTT::Connect( deviceID ).set_auth(mqttuser, mqttpass) );
      } else {
        success = client.connect( deviceID );
      }
      if (success) {
        client.set_callback(callback);
        Serial.println("Connect to MQTT server: Success");
        client.subscribe(prefix); // for receiving HELLO messages and handshaking
        pubConfig();
      } else {
        Serial.println("Connect to MQTT server: FAIL");   
        delay(1000);
      }
    }

    if (client.connected()) {
      newtime = millis();
      if (newtime - oldtime > 10000) { // read ADC and publish data every 10 sec
        stat[0] = setStatus( analogRead( pin[0] ) );
        pubStatus(sTopic[0], stat[0] ); 
        oldtime = newtime;
      }
      client.loop();
    }
  }
}
