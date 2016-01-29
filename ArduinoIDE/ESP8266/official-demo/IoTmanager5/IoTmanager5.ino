/*
 IoT Manager mqtt device client https://play.google.com/store/apps/details?id=ru.esp8266.iotmanager
 Based on Basic MQTT example with Authentication
 PubSubClient library v 1.91.1 https://github.com/Imroy/pubsubclient
  - connects to an MQTT server, providing userdescr and password
  - subscribes to the topic "/IoTmanager" (waiting "HELLO" messages from mobile device) 
  - publishes config to the topic "/IoTmanager/config/deviceID/"

  Tested with Arduino IDE 1.6.7 + ESP8266 Community Edition v 2.1.0-rc2 
  PubSubClient library v 1.91.1 https://github.com/Imroy/pubsubclient
  ArduinoJson library 5.0.7 https://github.com/bblanchon/ArduinoJson

  sketch version : 1.0
  IoT Manager    : 1.4.7 and above

  simple-btn widget demo, for details look at Widgets Guide on IoT Manager

<div class="class1" style="style1">
  <div class="class2" style="style2">
     descr
  </div>
  <button class="class3" style="style3">
    title
  </button>
</div>

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char *ssid =  "IoT";            // cannot be longer than 32 characters!
const char *pass =  "12345678";       // WiFi password

String prefix   = "/IoTmanager";      // global prefix for all topics - must be some as mobile device
String deviceID;

WiFiClient wclient;

// config for cloud mqtt broker by DNS hostname ( for example, cloudmqtt.com use: m20.cloudmqtt.com - EU )
String mqttServerName = "m20.cloudmqtt.com";            // for cloud broker - by hostname, from CloudMQTT account data
int    mqttport = 13191;                                // default 1883, but CloudMQTT.com use other, for example: 13191, 23191 (SSL), 33191 (WebSockets) - use from CloudMQTT account data
String mqttuser =  "test";                              // from CloudMQTT account data
String mqttpass =  "test";                              // from CloudMQTT account data
PubSubClient client(wclient, mqttServerName, mqttport); // for cloud broker - by hostname


// config for local mqtt broker by IP address
//IPAddress server(192, 168, 1, 100);                        // for local broker - by address
//int    mqttport = 9001;                                    // default 1883, but WebSockets port needed
//String mqttuser =  "";                                     // from broker config
//String mqttpass =  "";                                     // from broker config
//PubSubClient client(wclient, server, mqttport);            // for local broker - by address

int freeheap;
const int nWidgets = 3; // number of widgets
String sTopic      [nWidgets];
String stat        [nWidgets];
int    pin         [nWidgets];
String thing_config[nWidgets];

StaticJsonBuffer<1024> jsonBuffer;
JsonObject& json_status = jsonBuffer.createObject();

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
  String string_status;
  json_status.printTo(string_status);
  return string_status;
}
String setStatus ( int s ) {
  json_status["status"] = s;
  String string_status;
  json_status.printTo(string_status);
  return string_status;
}
void initVar() {

  deviceID = String( ESP.getChipId() ) + "-" + String( ESP.getFlashChipId() );     // IoT thing device ID - unique device id in our project

  // widget0
  JsonObject& root = jsonBuffer.createObject();
  sTopic[0] = prefix + "/" + deviceID + "/button_on";
  root["id"] = 0;
  root["page"] = "buttons";
  root["widget"] = "simple-btn";
  root["class1"] = "item no-border";                          // class for 1st div
  root["style1"] = "";                                        // style for 1st div
  root["descr"]  = "Button ON";                               // text  for description
  root["class2"] = "balanced";                                // class for description from Widgets Guide - Color classes
  root["style2"] = "font-size:20px;float:left;padding-top:10px;font-weight:bold;"; // style for description
  root["topic"] = sTopic[0];
  root["class3"] = "button button-balanced icon ion-power"; // class for button
  root["style3"] = "float:right;";                            // style for button
  root.printTo(thing_config[0]);

  // widget1
  JsonObject& root1 = jsonBuffer.createObject();
  sTopic[1] = prefix + "/" + deviceID + "/button_off";
  root1["id"] = 1;
  root1["page"] = "buttons";
  root1["widget"] = "simple-btn";
  root1["class1"] = "item no-border";                          // class for 1st div
  root1["style1"] = "";                                        // style for 1st div
  root1["descr"]  = "Button OFF";                              // text  for description
  root1["class2"] = "assertive";                               // class for description from Widgets Guide - Color classes
  root1["style2"] = "font-size:20px;float:left;padding-top:10px;font-weight:bold;"; // style for description
  root1["topic"] = sTopic[1];
  root1["class3"] = "button button-assertive icon ion-power"; // class for button
  root1["style3"] = "float:right;";                            // style for button
  root1.printTo(thing_config[1]);

  // widget2
  JsonObject& root2 = jsonBuffer.createObject();
  pin   [2] = 2;   // GPIO2 - LED attached
  sTopic[2] = prefix + "/" + deviceID + "/toggle";
  stat  [2] = setStatus (1); // LED off at startup
  root2["id"] = 2;
  root2["page"] = "buttons";
  root2["descr"]  = "Toggle";                   // text  for description
  root2["widget"] = "toggle";
  root2["color"] = "green";                     // black, blue, green, orange, red, white, yellow (off - grey)
  root2["topic"] = sTopic[2];

  root2.printTo(thing_config[2]);

  pinMode(pin[2], OUTPUT); // GPIO2 - LEN on ESP
  
}
void pubStatus(String t, String payload) {  
    if (client.publish(t + "/status", payload)) { 
       Serial.println("Publish new status to " + t + "/status" + ", value: " + payload);
    } else {
       Serial.println("Publish new status to " + t + "/status" + " FAIL!");
    }
    FreeHEAP(); // check memory leak
}
void pubConfig() {
  bool success;
  for (int i = 0; i < nWidgets; i = i + 1) {
      success = client.publish(MQTT::Publish(prefix + "/" + deviceID + "/config", thing_config[i]).set_qos(1));
      if (success) {
         Serial.println("Publish config: Success (" + thing_config[i] + ")");
      } else {
         Serial.println("Publish config FAIL! ("    + thing_config[i] + ")");
         delay(1000);
         ESP.restart();
      }
  }
  pubStatus(sTopic[2], stat[2]); // LED off at startup
}
void callback(const MQTT::Publish& sub) {

  Serial.print("Get data from subscribed topic ");
  Serial.print(sub.topic());
  Serial.print(" => ");
  Serial.println(sub.payload_string());

  if ( sub.payload_string() == "HELLO" ) {  // handshaking
     pubConfig();
  }

  if (sub.topic() == sTopic[0] + "/control") {   // control from toggle - widget id 0 - ON
    // payload from button always 1, change status only for widget toggle - id 2
    stat[2] = setStatus(1);   // 1-on 2-off LED on phone
    digitalWrite(pin[2],0);   // 0-on 1-off LED on device
    pubStatus(sTopic[2], stat[2]);
 } else if (sub.topic() == sTopic[1] + "/control") {   // control from simple-btn - widget id 1 - OFF
    // payload from button always 1, change status only for widget toggle - id 2
    stat[2] = setStatus(0);  // 1-on 2-off LED on phone
    digitalWrite(pin[2],1);  // 0-on 1-off LED on device
    pubStatus(sTopic[2], stat[2]);
 } else if (sub.topic() == sTopic[2] + "/control") {  // control from simple-btn - widget id 2 - ON
    // payload from button always 1, change status only for widget toggle - id 2
    if (sub.payload_string() == "0") {
       stat[2] = setStatus(0);
       digitalWrite(pin[2],1);
    } else {
       stat[2] = setStatus(1);
       digitalWrite(pin[2],0);
    }
    pubStatus(sTopic[2], stat[2]);
 }
}

void setup() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  initVar();
  Serial.begin(115200);
  delay(10);
  WiFi.printDiag(Serial);
  Serial.println();
  Serial.println();
  Serial.println("MQTT client started.");
  FreeHEAP();
  freeheap = 100000;
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
        client.subscribe(prefix);  // for receiving HELLO messages and handshaking
        client.subscribe(prefix + "/" + deviceID + "/+/control"); // subscribe to all "control" messages for all widgets of this device
        pubConfig();
      } else {
        Serial.println("Connect to MQTT server: FAIL");   
        delay(1000);
      }
    }
    if (client.connected()) {
      client.loop();
    }
  }
}
