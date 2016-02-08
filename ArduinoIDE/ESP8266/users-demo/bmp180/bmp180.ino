#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_BMP085.h> // https://github.com/vieiraf/bmp085driver
 
Adafruit_BMP085 bmp;
 
String davlen;
String temp;
const char *ssid =  "****";            // cannot be longer than 32 characters!
const char *pass =  "*****";       // WiFi password
 
String prefix   = "/IoTmanager";      // global prefix for all topics - must be some as mobile device
String deviceID = "dev04";            // thing ID - unique device id in our project
 
WiFiClient wclient;
 
// config for cloud mqtt broker by DNS hostname ( for example, cloudmqtt.com use: m20.cloudmqtt.com - EU, m11.cloudmqtt.com - USA )
String mqttServerName = "m20.cloudmqtt.com";            // for cloud broker - by hostname, from CloudMQTT account data
int    mqttport = 11536;                                // default 1883, but CloudMQTT.com use other, for example: 13191, 23191 (SSL), 33191 (WebSockets) - use from CloudMQTT account data
String mqttuser =  "*****";                              // from CloudMQTT account data
String mqttpass =  "*****";                              // from CloudMQTT account data
PubSubClient client(wclient, mqttServerName, mqttport); // for cloud broker - by hostname
 
 
// config for local mqtt broker by IP address
//IPAddress server(192, 168, 1, 100);                        // for local broker - by address
//int    mqttport = 1883;                                    // default 1883
//String mqttuser =  "test";                                     // from broker config
//String mqttpass =  "test";                                     // from broker config
//PubSubClient client(wclient, server, mqttport);            // for local broker - by address
 
String val;
String ids = "";
int newValue, newtime, oldtime, freeheap;
 
const int nWidgets = 2;
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
 
  sTopic[0] = prefix + "/" + deviceID + "/BMP180_temp";
  stat  [0] = setStatus (0);
  sTopic[1] = prefix + "/" + deviceID + "/BMP180_davlen";
  stat  [1] = setStatus (1);
 
  JsonObject& root = jsonBuffer.createObject();
  JsonObject& cfg  = jsonBuffer.createObject();
 
  root["id"] = 0;
  root["page"] = "BMP180";
  root["widget"] = "display-value";
  root["class1"] = "item no-border";                          // class for 1st div
  root["style1"] = "";                                        // style for 1st div
  root["descr"]  = "Температура";                            // text  for description
  root["class2"] = "balanced";                                // class for description from Widgets Guide - Color classes
  root["style2"] = "font-size:20px;float:left;padding-top:10px;font-weight:bold;"; // style for description
  root["topic"] = sTopic[0];
  root["class3"] = "";                                        // class for 3 div - SVG
  root["style3"] = "float:right;";                            // style for 3 div - SVG
  root["height"] = "40";                                      // SVG height without "px"
  root["color"]  = "#52FF00";                                 // color for active segments
  root["inactive_color"] = "#414141";                         // color for inactive segments
  root["digits_count"]   = 6;                                 // how many digits
 
  root.printTo(thing_config[0]);
 
/////
 
  root["id"] = 1;
  root["page"] = "BMP180";
  root["widget"] = "display-value";
  root["class1"] = "item no-border";                          // class for 1st div
  root["style1"] = "";                                        // style for 1st div
  root["descr"]  = "Давление (мм.рт.ст)";                            // text  for description
  root["class2"] = "balanced";                                // class for description from Widgets Guide - Color classes
  root["style2"] = "font-size:20px;float:left;padding-top:10px;font-weight:bold;"; // style for description
  root["topic"] = sTopic[1];
  root["class3"] = "";                                        // class for 3 div - SVG
  root["style3"] = "float:right;";                            // style for 3 div - SVG
  root["height"] = "40";                                      // SVG height without "px"
  root["color"]  = "#52FF00";                                 // color for active segments
  root["inactive_color"] = "#414141";                         // color for inactive segments
  root["digits_count"]   = 3;                                 // how many digits
 
  root.printTo(thing_config[1]);
//////////////////////////////////////
 
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
  Wire.pins(0, 2);
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085 sensor, check wiring!");
  while (1) {}
  }
  delay(50);
        temp = String(bmp.readTemperature());
        davlen= String(bmp.readPressure()/133);
        stat[0] = setStatus( temp );
        stat[1] = setStatus( davlen );
 
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
      if (newtime - oldtime > 10000) { // publish data every 100 sec
     
     
        temp = String(bmp.readTemperature());
        davlen= String(bmp.readPressure()/133);
        stat[0] = setStatus( temp );
        stat[1] = setStatus( davlen );
          for (int i = 0; i < nWidgets; i = i + 1) {
            pubStatus(sTopic[i], stat[i]);
            delay(100);
          }
        oldtime = newtime;
      }
      client.loop();
    }
  }
}
