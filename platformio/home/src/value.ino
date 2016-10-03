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

  anydata widget demo, for details look at http://iotmanager.ru

<div class="class1" style="style1">
  <div class="class2" style="style2">
     descr
  </div>
  <div class="class3" style="style3">
    any data from device
  </div>
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
String mqttServerName = "m11.cloudmqtt.com";            // for cloud broker - by hostname, from CloudMQTT account data
int    mqttport = 10927;                                // default 1883, but CloudMQTT.com use other, for example: 13191, 23191 (SSL), 33191 (WebSockets) - use from CloudMQTT account data
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
String sTopic      [nWidgets];
String stat        [nWidgets];
int    pin         [nWidgets];
String thing_config[nWidgets];

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

  sTopic[0] = prefix + "/" + deviceID + "/anydata0";
  root["id"] = 0;
  root["widget"] = "anydata";
  root["topic"]  = sTopic[0];
  root["class1"] = "item no-border text-center";              // class for 1st div
  root["descr"]  = "Kitchen temp";                            // text  for description
  root["class2"] = "balanced";                                // class for description from Widgets Guide - Color classes
  root["style2"] = "font-size:20px;font-weight:bold;";        // style for description
  root.printTo(thing_config[0]);

  // widget1
  JsonObject& root1 = jsonBuffer.createObject();
  sTopic[1] = prefix + "/" + deviceID + "/anydata1";
  root1["id"] = 1;
  root1["widget"] = "anydata";
  root1["topic"] = sTopic[1];
  root1["class1"] = "item no-border";                          // class for 1st div
  root1["descr"]  = "Current temp";                            // text  for description
  root1["class2"] = "assertive";                               // class for description from Widgets Guide - Color classes
  root1["style2"] = "float:left;";                             // style for description
  root1["class3"] = "light assertive-bg padding-left padding-right rounded";      // class for value
  root1["style3"] = "float:right;font-weight:bold;";           // style for value
  root1.printTo(thing_config[1]);

  view = 0;
}
void pubStatus(String t, float temp) {

    String payload;

    i ++;   // we change style

    if (i == 1) {
        payload = "{\"status\":\"" + String(temp) + "\"}";
    } else if ( i == 2) {
       payload = "{\"status\":\"" + String(temp) + "\", \"class3\": \"light assertive-bg padding-left padding-right rounded\" }";
    } else if ( i == 3) {
       payload = "{\"status\":\"" + String(temp) + "\", \"class3\": \"dark light-bg padding-left padding-right rounded\" }";
    } else if ( i == 4) {
       payload = "{\"status\":\"" + String(temp) + "\", \"class3\": \"light padding assertive-bg rounded\" }";
    } else if ( i == 5) {
       payload = "{\"status\":\"" + String(temp) + "\", \"class3\": \"light calm-bg padding-left padding-right rounded\" }";
    } else if ( i == 6) {
       payload = "{\"status\":\"" + String(temp) + "\", \"style2\": \"float:right;\",\"style3\":\"float:left\"}";
    } else if ( i == 7) {
       payload = "{\"status\":\"" + String(temp) + "\", \"descr\": \"Another text\" }";
    } else if ( i == 8) {
       payload = "{\"status\":\"" + String(temp) + "\", \"style2\": \"font-size:40px;float:right;\" }";
    } else if ( i == 9) {
       payload = "{\"status\":\"" + String(temp) + "\", \"style2\": \"float:right;\" }";
    } else if ( i == 10) {
       payload = "{\"status\":\"" + String(temp) + "\", \"style2\": \"float:left;\",\"style3\":\"float:right\"}";
    } else if ( i == 11) {
       payload = "{\"status\":\"" + String(temp) + "\", \"style3\": \"font-size:20px;float:right;\" }";
    } else if ( i == 12) {
       payload = "{\"status\":\"" + String(temp) + "\", \"class1\": \"item\" }";
    } else if ( i == 13) {
       payload = "{\"status\":\"" + String(temp) + "\", \"class1\": \"item no-border\" }";
    } else if ( i == 14) {
       payload = "{\"status\":\"" + String(temp) + "\", \"style2\": \"\", \"style3\":\"\", \"class3\":\"text-center\" }";
    } else if ( i == 15) {
       payload = "{\"status\":\"" + String(temp) + "\", \"style2\": \"display:none\", \"class3\":\"button button-positive\" }";
    } else if ( i == 16) {
       payload = "{\"status\":\"" + String(temp) + "\", \"style2\": \"\", \"style3\":\"display:none;\" }";
    } else if ( i == 17) {
       payload = "{\"status\":\"" + String(temp) + "\", \"style2\": \"float:left;\",\"style3\":\"float:right\"}";
    } else if ( i == 18) {
       payload = "{\"status\":\"" + String(temp) + "\", \"class2\": \"light energized-bg padding\",\"class3\":\"balanced-100\",\"style3\":\"float:right;font-size:40px;font-weight:bold;\"}";
    } else if ( i == 19) {
       payload = "{\"status\":\"" + String(temp) + "\", \"style2\": \"float:left;\",\"style3\":\"float:right\",\"class2\":\"energized\",\"class3\":\"light\"}";
    } else if ( i == 20) {
       payload = "{\"status\":\"" + String(temp) + "\", \"style2\": \"float:left;\",\"style3\":\"float:right;\"}";
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
  float tempC = random(-40,+40);

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
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting via WiFi to ");
    Serial.print(ssid);
    Serial.println("...");

    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.print(".");
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

      newtime = millis();
      if (newtime - oldtime > 1000) { // 1 sec
        pubTemperature();
        oldtime = newtime;
      }

      client.loop();
    }
  }
  yield();

}
