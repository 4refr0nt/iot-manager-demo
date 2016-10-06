/*
 IoT Manager mqtt device client 
 
 https://play.google.com/store/apps/details?id=ru.esp8266.iotmanager
 
 Based on Basic MQTT example with Authentication
 
 PubSubClient library v 1.91.1 https://github.com/Imroy/pubsubclient
  - connects to an MQTT server, providing userdescr and password
  - publishes config to the topic "/IoTmanager/config/deviceID/"
  - subscribes to the topic "/IoTmanager/hello" ("hello" messages from mobile device) 

  Tested with Arduino IDE 1.6.12 + ESP8266 Community Edition v 2.3.0
  
  PubSubClient library v 1.91.1 https://github.com/Imroy/pubsubclient
  
  sketch version : 1.6
  IoT Manager    : any version

  toggle, range, small-badge widgets demo
*/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

const char *ssid =  "IoT";            // cannot be longer than 32 characters!
const char *pass =  "12345678";       // WiFi password

String prefix   = "/IoTmanager";     // global prefix for all topics - must be some as mobile device
String deviceID = "dev01-kitchen";   // thing ID - unique device id in our project

WiFiClient wclient;

// config for cloud mqtt broker by DNS hostname ( for example, cloudmqtt.com use: m20.cloudmqtt.com - EU, m11.cloudmqtt.com - USA )
//String mqttServerName = "m11.cloudmqtt.com";            // for cloud broker - by hostname, from CloudMQTT account data
//int    mqttport = 10927;                                // default 1883, but CloudMQTT.com use other, for example: 13191, 23191 (SSL), 33191 (WebSockets) - use from CloudMQTT account data
//String mqttuser =  "test";                              // from CloudMQTT account data
//String mqttpass =  "test";                              // from CloudMQTT account data
//PubSubClient client(wclient, mqttServerName, mqttport); // for cloud broker - by hostname


// config for local mqtt broker by IP address
IPAddress server(192, 168, 1, 135);                        // for local broker - by address
int    mqttport = 1883;                                    // default 1883
String mqttuser =  "";                                     // from broker config
String mqttpass =  "";                                     // from broker config
PubSubClient client(wclient, server, mqttport);            // for local broker - by address

String val;
String ids = "";
int oldtime, newtime, pushtime;
int newValue;

const String stat1 = "{\"status\":\"1\"}";
const String stat0 = "{\"status\":\"0\"}";

const int nWidgets = 8;
String stat        [nWidgets];
String sTopic      [nWidgets];
String color       [nWidgets];
String style       [nWidgets];
String badge       [nWidgets];
String widget      [nWidgets];
String descr       [nWidgets];
String page        [nWidgets];
String pageId      [nWidgets];
String thing_config[nWidgets];
String id          [nWidgets];
int    pin         [nWidgets];
int    defaultVal  [nWidgets];
bool   inverted    [nWidgets];

String setStatus ( String s ) {
  String stat = "{\"status\":\"" + s + "\"}";
  return stat;
}
String setStatus ( int s ) {
  String stat = "{\"status\":\"" + String(s) + "\"}";
  return stat;
}
void initVar() {
  id    [0] = "0";
  page  [0] = "Kitchen";
  pageId[0] = 1;
  descr [0] = "Light-0";
  widget[0] = "toggle";
  pin[0] = 4;                                              // GPIO4 - toggle
  defaultVal[0] = 1;                                       // defaultVal status
  inverted[0] = true;
  sTopic[0]   = prefix + "/" + deviceID + "/light0";
  color[0]   = "\"color\":\"blue\"";                       // black, blue, green, orange, red, white, yellow (off - grey)

  id    [1] = "1";
  page  [1] = "Kitchen";
  pageId[1] = 1;
  descr [1] = "Light-1";
  widget[1] = "toggle";
  pin[1] = 5;                                              // GPIO5 - toggle
  defaultVal[1] = 1;                                       // defaultVal status
  inverted[1] = true;
  sTopic[1] = prefix + "/" + deviceID + "/light1";
  color [1] = "\"color\":\"orange\"";                       // black, blue, green, orange, red, white, yellow (off - grey)

  id    [2] = "2";
  page  [2] = "Kitchen";
  pageId[2] = 1;
  descr [2] = "Dimmer";
  widget[2] = "range";
  pin[2] = 0;                                              // GPIO0 - range
  defaultVal[2] = 1023;                                    // defaultVal status 0%, inverted
  inverted[1] = true;
  sTopic[2]   = prefix + "/" + deviceID + "/dim-light";
  style[2]   = "\"style\":\"range-calm\"";                 // see http://ionicframework.com/docs/components/#colors
  badge[2]   = "\"badge\":\"badge-assertive\"";            // see http://ionicframework.com/docs/components/#colors

  id    [3] = "3";
  page  [3] = "Kitchen";
  pageId[3] = 1;
  descr  [3] = "ADC";
  widget[3] = "small-badge";
  pin   [3] = A0;                                          // ADC
  sTopic[3] = prefix + "/" + deviceID + "/ADC";
  badge [3] = "\"badge\":\"badge-balanced\"";             // see http://ionicframework.com/docs/components/#colors

  id    [4] = "4";
  page  [4] = "Outdoor";
  pageId[4] = 2;
  descr [4] = "Garden light";
  widget[4] = "toggle";
  pin   [4] = 2;                                           // GPIO2
  defaultVal[4] = 1;                                       // defaultVal status 0%
  inverted[4] = true;
  sTopic[4] = prefix + "/" + deviceID + "/light4";     
  color [4] = "\"color\":\"red\"";                         // black, blue, green, orange, red, white, yellow (off - grey)

  // RED
  id    [5] = "5";
  page  [5] = "Kitchen";
  pageId[5] = 1;
  descr [5] = "RED";
  widget[5] = "range";
  pin   [5] = 15;                                          // GPIO15 - range
  defaultVal[5] = 0;                                       // defaultVal 0%, not inverted
  sTopic[5]   = prefix + "/" + deviceID + "/red";
  style[5]   = "\"style\":\"range-assertive\"";            // see http://ionicframework.com/docs/components/#colors
  badge[5]   = "\"badge\":\"badge-assertive\"";            // see http://ionicframework.com/docs/components/#colors

  // GREEN
  id    [6] = "6";
  page  [6] = "Kitchen";
  pageId[6] = 1;
  descr [6] = "GREEN";
  widget[6] = "range";
  pin[6] = 12;                                      // GPIO12 - range
  defaultVal[6] = 0;                                       // defaultVal 0%
  sTopic[6]   = prefix + "/" + deviceID + "/green";
  style[6]   = "\"style\":\"range-balanced\"";         // see http://ionicframework.com/docs/components/#colors
  badge[6]   = "\"badge\":\"badge-balanced\"";          // see http://ionicframework.com/docs/components/#colors

  // BLUE
  id    [7] = "7";
  page  [7] = "Kitchen";
  pageId[7] = 1;
  descr [7] = "BLUE";
  widget[7] = "range";
  pin[7] = 13;                                      // GPIO13 - range
  defaultVal[7] = 0;                                       // defaultVal status 0%
  sTopic[7]  = prefix + "/" + deviceID + "/blue";
  style[7]   = "\"style\":\"range-calm\"";             // see http://ionicframework.com/docs/components/#colors
  badge[7]   = "\"badge\":\"badge-calm\"";              // see http://ionicframework.com/docs/components/#colors

  for (int i = 0; i < nWidgets; i++) {
    if (inverted[i]) {
      if (defaultVal[i]>0) {
         stat[i] = setStatus(0);
      } else {
         stat[i] = setStatus(1);
      }
    } else {
       stat[i] = setStatus(defaultVal[i]);
    }
  }      

  thing_config[0] = "{\"id\":\"" + id[0] + "\",\"page\":\"" + page[0]+"\",\"descr\":\"" + descr[0] + "\",\"widget\":\"" + widget[0] + "\",\"topic\":\"" + sTopic[0] + "\"," + color[0] + "}";   // GPIO switched On/Off by mobile widget toggle
  thing_config[1] = "{\"id\":\"" + id[1] + "\",\"page\":\"" + page[1]+"\",\"descr\":\"" + descr[1] + "\",\"widget\":\"" + widget[1] + "\",\"topic\":\"" + sTopic[1] + "\"," + color[1] + "}";   // GPIO switched On/Off by mobile widget toggle
  thing_config[2] = "{\"id\":\"" + id[2] + "\",\"page\":\"" + page[2]+"\",\"descr\":\"" + descr[2] + "\",\"widget\":\"" + widget[2] + "\",\"topic\":\"" + sTopic[2] + "\"," + style[2] + "," + badge[2] + ",\"leftIcon\":\"ion-ios-rainy-outline\",\"rightIcon\":\"ion-ios-rainy\"}"; // for icons see http://ionicons.com
  thing_config[3] = "{\"id\":\"" + id[3] + "\",\"page\":\"" + page[3]+"\",\"descr\":\"" + descr[3] + "\",\"widget\":\"" + widget[3] + "\",\"topic\":\"" + sTopic[3] + "\"," + badge[3] + "}";   // ADC
  thing_config[4] = "{\"id\":\"" + id[4] + "\",\"page\":\"" + page[4]+"\",\"descr\":\"" + descr[4] + "\",\"widget\":\"" + widget[4] + "\",\"topic\":\"" + sTopic[4] + "\"," + color[4] + "}";   // GPIO switched On/Off by mobile widget toggle
  thing_config[5] = "{\"id\":\"" + id[5] + "\",\"page\":\"" + page[5]+"\",\"descr\":\"" + descr[5] + "\",\"widget\":\"" + widget[5] + "\",\"topic\":\"" + sTopic[5] + "\"," + style[5] + ","+ badge[5] + "}";    // GPIO15 R
  thing_config[6] = "{\"id\":\"" + id[6] + "\",\"page\":\"" + page[6]+"\",\"descr\":\"" + descr[6] + "\",\"widget\":\"" + widget[6] + "\",\"topic\":\"" + sTopic[6] + "\"," + style[6] + ","+ badge[6] + "}";    // GPIO12 G
  thing_config[7] = "{\"id\":\"" + id[7] + "\",\"page\":\"" + page[7]+"\",\"descr\":\"" + descr[7] + "\",\"widget\":\"" + widget[7] + "\",\"topic\":\"" + sTopic[7] + "\"," + style[7] + ","+ badge[7] + "}";    // GPIO13 B

}
// send confirmation
void pubStatus(String t, String payload) {  
    if (client.publish(t + "/status", payload)) { 
       Serial.println("Publish new status for " + t + ", value: " + payload);
    } else {
       Serial.println("Publish new status for " + t + " FAIL!");
    }
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
        delay(50);
      }      
  }
  if (success) {
     Serial.println("Publish config: Success");
  } else {
     Serial.println("Publish config: FAIL");
  }
  for (int i = 0; i < nWidgets; i = i + 1) {
      pubStatus(sTopic[i], stat[i]);
      delay(50);
  }      
}


void callback(const MQTT::Publish& sub) {
  Serial.print("Get data from subscribed topic ");
  Serial.print(sub.topic());
  Serial.print(" => ");
  Serial.println(sub.payload_string());

  if (sub.topic() == sTopic[0] + "/control") {
    if (sub.payload_string() == "0") {
       newValue = 1; // inverted
       stat[0] = stat0;
    } else {
       newValue = 0;
       stat[0] = stat1;
    }
    digitalWrite(pin[0],newValue);
    pubStatus(sTopic[0], stat[0]);
 } else if (sub.topic() == sTopic[1] + "/control") {
    if (sub.payload_string() == "0") {
       newValue = 1; // inverted
       stat[1] = stat0;
    } else {
       newValue = 0; // inverted
       stat[1] = stat1;
    }
    digitalWrite(pin[1],newValue);
    pubStatus(sTopic[1], stat[1]);
 } else if (sub.topic() == sTopic[2] + "/control") {
    String x = sub.payload_string();
    analogWrite(pin[2],1023-x.toInt());
    stat[2] = setStatus(x);
    pubStatus(sTopic[2], stat[2]);
 } else if (sub.topic() == sTopic[3] + "/control") {
   // ADC : nothing, display only
 } else if (sub.topic() == sTopic[4] + "/control") {
    if (sub.payload_string() == "0") {
       newValue = 1; // inverted
       stat[4] = stat0;
    } else {
       newValue = 0; // inverted
       stat[4] = stat1;
    }
    digitalWrite(pin[4],newValue);
    pubStatus(sTopic[4], stat[4]);
 } else if (sub.topic() == sTopic[5] + "/control") {
    String x = sub.payload_string();
    analogWrite(pin[5],x.toInt());
    stat[5] = setStatus(x);
    pubStatus(sTopic[5], stat[5]);
 } else if (sub.topic() == sTopic[6] + "/control") {
    String x = sub.payload_string();
    analogWrite(pin[6],x.toInt());
    stat[6] = setStatus(x);
    pubStatus(sTopic[6], stat[6]);
 } else if (sub.topic() == sTopic[7] + "/control") {
    String x = sub.payload_string();
    analogWrite(pin[7],x.toInt());
    stat[7] = setStatus(x);
    pubStatus(sTopic[7], stat[7]);
 } else if (sub.topic() == prefix) {
    if (sub.payload_string() == "HELLO") {
      pubConfig();
    }
 }
}

void setup() {
  initVar();
  WiFi.mode(WIFI_STA);
  pinMode(pin[0], OUTPUT);
  digitalWrite(pin[0],defaultVal[0]);
  pinMode(pin[1], OUTPUT);
  digitalWrite(pin[1],defaultVal[1]);
  pinMode(pin[2], OUTPUT);
  analogWrite(pin[2],defaultVal[2]);  // PWM
  stat[3] = setStatus(analogRead(pin[3]));
  pinMode(pin[4], OUTPUT);
  digitalWrite(pin[4],defaultVal[4]);
  pinMode(pin[5], OUTPUT);
  analogWrite(pin[5],defaultVal[5]);  // PWM
  pinMode(pin[6], OUTPUT);
  analogWrite(pin[6],defaultVal[6]);  // PWM
  pinMode(pin[7], OUTPUT);
  analogWrite(pin[7],defaultVal[7]);  // PWM
  // Setup console
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.println("MQTT client started.");
  Serial.print("Free heap = ");
  Serial.println(ESP.getFreeHeap());
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
        pubConfig();
	      client.subscribe(prefix);                  // for receiving HELLO messages
        client.subscribe(prefix + "/+/+/control"); // for receiving GPIO control messages
        Serial.println("Subscribe: Success");
      } else {
        Serial.println("Connect to MQTT server: FAIL");   
        delay(1000);
      }
    }

    if (client.connected()) {
      newtime = millis();
      if (newtime - oldtime > 10000) { // 10 sec
        int x = analogRead(pin[3]);
        val = "{\"status\":\"" + String(x)+ "\"}";
        client.publish(sTopic[3] + "/status", val );  // widget 3
        oldtime = newtime;
      }
      client.loop();
    }
  }
}
