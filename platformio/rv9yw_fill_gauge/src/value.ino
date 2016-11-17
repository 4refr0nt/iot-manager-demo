/*
  IoT Manager mqtt demo device client https://play.google.com/store/apps/details?id=ru.esp8266.iotmanager

  Tested with Platformio

  PubSubClient library v 1.91.1 https://github.com/Imroy/pubsubclient
  (use commaind: "platformio lib install https://github.com/Imroy/pubsubclient.git")

  ArduinoJson library
  (use commaind: "platformio lib install ArduinoJson")



  sketch version : 1.0
  author         : 4refr0nt
  IoT Manager    : 1.5.0 and above
        Widget - fillgauge.

          <div style="style1" class="class1">
              <div style="style2" class="class2">
                 descr
              </div>
              <div style="style3" class="class3">
                  <svg width="w" height="h"></svg>
              </div>
          </div>

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const int nWiFi = 2;
char *ssid[] = {"first_SSID","second_SSID"};
char *pass[] = {"0102030405","0102030405"};       // WiFi password

String prefix   = "/IoTmanager";      // global prefix for all topics - must be some as mobile device
String deviceID = "dev01";

WiFiClient wclient;

// config for cloud mqtt broker by DNS hostname ( for example, cloudmqtt.com use: m20.cloudmqtt.com - EU )
String mqttServerName = "m11.cloudmqtt.com";            // for cloud broker - by hostname, from CloudMQTT account data
int    mqttport = 13161;                                // default 1883, but CloudMQTT.com use other, for example: 13191, 23191 (SSL), 33191 (WebSockets) - use from CloudMQTT account data
String mqttuser =  "test";                              // from CloudMQTT account data
String mqttpass =  "test";                              // from CloudMQTT account data
PubSubClient client(wclient, mqttServerName, mqttport); // for cloud broker - by hostname

// config for local mqtt broker by IP address
// IPAddress server(192, 168, 1, 135);                        // for local broker - by address
// int    mqttport = 1883;                                    // default 1883, but WebSockets port needed
// String mqttuser = "test";                                  // from broker config
// String mqttpass = "test";                                  // from broker config
// PubSubClient client(wclient, server, mqttport);           // for local broker - by address
//

int devices, newtime, oldtime, view, i;

const int nWidgets = 2; // number of widgets
String sTopic       [nWidgets];
String stat         [nWidgets];
int    pin          [nWidgets];
String thing_config [nWidgets];

void restart() {
   Serial.println();
   Serial.println("Restarting after 5 sec...");
   delay(5000);
   ESP.restart();
}

void initVar() {

  StaticJsonBuffer<1024> jsonBuffer;

  i = 0;
  deviceID = String( ESP.getChipId() ) + "-" + String( ESP.getFlashChipId() );     // IoT thing device ID - unique device id in our project

  // widget0 - simple string
  JsonObject& root = jsonBuffer.createObject();

  sTopic[0] = prefix + "/" + deviceID + "/fillgauge0";
  root["id"] = 0;
  root["page"]   = "Fill Gauge";
  root["pageId"] = 10;
  root["widget"] = "fillgauge";
  root["topic"]  = sTopic[0];
  root["class1"] = "item no-border text-center";              // class for 1st div
  root["descr"]  = "Tank of Oil";                            // text  for description
  root["class2"] = "text-center";                  // class for description from Widgets Guide - Color classes
  root["style2"] = "font-size:14px;font-weight:bold;padding-bottom:5px;";        // style for description
  root["width"] =  "150px";
  root["height"] = "150px";

  JsonObject& rootExt = root.createNestedObject("widgetConfig");

  rootExt["circleColor"] = "#22ff55";
  rootExt["textColor"]   = "#33ff44";
  rootExt["wavColor"]     = "#AAAA39";
  rootExt["textSize"]    = 0.75;

  root.printTo(thing_config[0]);

  // widget1
  JsonObject& root1 = jsonBuffer.createObject();

  sTopic[1] = prefix + "/" + deviceID + "/fillgauge1";

  root1["id"] = 1;
  root1["page"]   = "Fill Gauge";
  root1["pageId"] = 10;
  root1["widget"] = "fillgauge";
  root1["topic"] = sTopic[1];
  root1["class1"] = "item no-border text-center";                          // class for 1st div
  root1["descr"]  = "Tank of Water";                            // text  for description
  root1["class2"] = "";                               // class for description from Widgets Guide - Color classes
  root1["style2"] = "font-size:14px;font-weight:bold;padding-bottom:5px;"; // style for description
  root1["class3"] = "";      // class for value
  root1["style3"] = "";           // style for value
  root1["width"] =  "200px";
  root1["height"] = "200px";

  JsonObject& root1Ext = root1.createNestedObject("widgetConfig");

  root1Ext["circleColor"]   = "#808015";
  root1Ext["textColor"]     = "#555500";
  root1Ext["waveTextColor"] = "#FFFFAA";
  root1Ext["wavColor"]     = "#AAAA39";
  root1Ext["circleThickness"] = 0.15;
  root1Ext["textVertPosition"] = 0.8;
  root1Ext["waveAnimateTime"]  = 1000;
  root1Ext["waveHeight"] = 0.05;
  root1Ext["waveAnimate"] = true;
  root1Ext["waveRise"]  = false;
  root1Ext["waveHeightScaling"]  = false;
  root1Ext["waveOffset"] = 0.25;
  root1Ext["textSize"] = 0.75;
  root1Ext["waveCount"]  = 3;

  root1.printTo(thing_config[1]);

 view = 0;
}
void pubStatus(String t, float temp) {

    String payload;

    i ++;   // we change style

    if (i == 1) {
      payload = "{\"status\":\"" + String(temp) + "\" }";
    } else if ( i == 1) {
      payload = "{\"status\":\"" + String(temp) + "\", \"vibrate\": 50" +  "}";
    } else if ( i == 2) {
      payload = "{\"status\":\"" + String(temp) + "\", \"toastLong\":\"Alert!\"" + "}";
    } else if ( i == 3) {
      payload = "{\"status\":\"" + String(temp) + "\", \"beep\": 1" + "}";
      i = 0;
    }

    if (client.publish(t + "/status", payload)) {
       Serial.println("Publish new status to " + t + "/status" + ", value: " + payload);
    } else {
       Serial.println("Publish new status to " + t + "/status" + " FAIL!");
       restart();
    }
}
void pubConfig() {
  bool success;
  for (int i = 0; i < nWidgets; i = i + 1) {
      success = client.publish(MQTT::Publish(prefix + "/" + deviceID + "/config", thing_config[i]).set_qos(1));
      if (success) {
         Serial.println("Publish config: Success (" + thing_config[i] + ")");
      } else {
         Serial.println("Publish config FAIL! ("    + thing_config[i] + ")");
         restart();
      }
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

void pubTemperature()
{

  // at this point you can read value from sensors
  float tempC = random(+5,+100);
  pubStatus( sTopic[0], tempC );
  pubStatus( sTopic[1], tempC );
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
  Serial.println("MQTT demo client started.");

}

void loop() {
  int  i = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting via WiFi to ");
    Serial.print(ssid[i]);
    Serial.println("...");
    WiFi.begin(ssid[i], pass[i]);

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      i=i+1;
      if (i==nWiFi) i = 0;
      Serial.println("WiFi connect: Fail");
      continue;
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

      newtime = millis();
      if (newtime - oldtime > 3000) { // 1 sec
        pubTemperature();
        oldtime = newtime;
      }

      client.loop();
    }
  }
  yield();

}
