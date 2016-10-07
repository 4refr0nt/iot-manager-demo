/*

  IoT Manager mqtt device client

  https://play.google.com/store/apps/details?id=ru.esp8266.iotmanager
  https://itunes.apple.com/us/app/iot-manager/id1155934877

  Based on Basic MQTT example with Authentication

  Tested with Arduino IDE   1.6.12 http://arduino.cc
  ESP8266 Community Edition 2.3.0  https://github.com/esp8266/Arduino
  PubSubClient library      1.91.1 https://github.com/Imroy/pubsubclient

  sketch version : 1.0
  IoT Manager    : any version

  toggle and anydata demo
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "anydata_0.h"
#include "anydata_1.h"
#include "toggle_2.h"

const char *ssid =  "IoT";            // cannot be longer than 32 characters!
const char *pass =  "12345678";       // WiFi password

String prefix   = "/IoTmanager";      // global prefix for all topics - must be some as mobile device
String deviceId = "dev22";            // thing Id - unique device id in our project. If change this - change in JSON too.

WiFiClient wclient;

// config for cloud mqtt broker by DNS hostname ( for example, cloudmqtt.com use: m20.cloudmqtt.com - EU, m11.cloudmqtt.com - USA )

String mqttServerName = "m11.cloudmqtt.com";            // for cloud broker - by hostname, from CloudMQTT account data
int    mqttport = 10927;                                // default 1883, but CloudMQTT.com use other, for example: 13191, 23191 (SSL), 33191 (WebSockets) - use from CloudMQTT account data
String mqttuser =  "test";                              // from CloudMQTT account data
String mqttpass =  "test";                              // from CloudMQTT account data
PubSubClient client(wclient, mqttServerName, mqttport); // for cloud broker - by hostname
#define PUB_DELAY 0                                     // recommended value 0 for local mosquitto broker and from 0 to 50 for remote cloud broker

// config for local mqtt broker by IP address
//IPAddress server(192, 168, 1, 135);                        // for local broker - by address
//int    mqttport = 1883;                                    // default 1883
//String mqttuser =  "test";                                 // from broker config
//String mqttpass =  "test";                                 // from broker config
//PubSubClient client(wclient, server, mqttport);            // for local broker - by address
//#define PUB_DELAY 0                                        // recommended value 0 for local mosquitto broker and 50 for remote cloud broker

String val;
int timer10s, timer1s, timeNow;
int newValue;
bool success, needPubConfig, needControlGPIO;

const int nWidgets = 3;
String stat [nWidgets];

#define RELAY_PIN 2   // toggle pin GPIO_2

String topic = "";
String payload = "";

String setStatus ( String s ) {
  String stat = "{\"status\":\"" + s + "\"}";
  return stat;
}

String setStatus ( String s, int vibrate ) {

  String stat;

  if (vibrate > 0) {

    stat = "{\"status\" :" + s + ", \"vibrate\": " + String(vibrate) + "}";

  } else {

    stat = "{\"status\" :" + s + "}";

  }

  return stat;
}

String setStatus ( int s ) {
  String stat = "{\"status\" :" + String(s) + "}";
  return stat;
}

String setStatus ( int s, int vibrate ) {

  String stat;

  if (vibrate > 0) {

    stat = "{\"status\" :" + String(s) + ", \"vibrate\":" + String(vibrate) + "}";

  } else {

    stat = "{\"status\" :" + String(s) + "}";

  }
  return stat;
}

void initVar() {
  // initial status for widget 0 in config
  stat[1] = setStatus(0);  // initial status for widget 1
  stat[2] = setStatus(0);  // initial status for widget 2 - LED OFF
}

// send confirmation - status message
void pubStatus(String t, String payload) {

  if (client.publish(t + "/status", payload)) {
    Serial.println("Publish new status for " + t + ", value: " + payload);
  } else {
    Serial.println("Publish new status for " + t + " FAIL!");
  }

  delay(PUB_DELAY);
}

// send widgets - config message
void pubConfig() {

  needPubConfig = false;

  bool success;

  success = client.publish(MQTT::Publish(prefix + "/" + deviceId + "/config", anydata_0_json).set_qos(1));
  // status for widget0 in config

  if (!success) {
    Serial.println("Publish config 0 FAIL!");
    return;
  }

  delay(PUB_DELAY);
  success = client.publish(MQTT::Publish(prefix + "/" + deviceId + "/config", anydata_1_json).set_qos(1));

  if (!success) {
    Serial.println("Publish config 1 FAIL!");
    return;
  }

  delay(PUB_DELAY);
  pubStatus(prefix + "/" + deviceId + "/widget1", stat[1]);

  delay(PUB_DELAY);
  success = client.publish(MQTT::Publish(prefix + "/" + deviceId + "/config", toggle_2_json).set_qos(1));

  if (!success) {
    Serial.println("Publish config 2 FAIL!");
    return;
  }

  delay(PUB_DELAY);
  pubStatus(prefix + "/" + deviceId + "/widget2", stat[2]);

  Serial.println("Publish config and status: Success.");
  yield();
}

void getSensorData() {

  // widget 1 - ADC

  int x = analogRead(A0);
  stat[1] = setStatus(x);
  pubStatus(prefix + "/" + deviceId + "/widget1", stat[1]);

}

void controlGPIO () {

  String t = topic;
  String p = payload;

  needControlGPIO = false;

  if (t == prefix + "/" + deviceId + "/widget2/control") {

    Serial.println("\nReceived relay command: " + p);

    stat[2] = setStatus(p, 50);        // add vibrate

    if (p == "0") {
      digitalWrite(RELAY_PIN, 1);      // inverted
    } else {
      digitalWrite(RELAY_PIN, 0);      // inverted
    }

    pubStatus(prefix + "/" + deviceId + "/widget2", stat[2]);

  } else {

    Serial.println();
    Serial.println("Unknown control message arrived: " + t + " => " + p);

  }
}

void onMessageArrived(const MQTT::Publish& sub) {

  topic = sub.topic();
  payload = sub.payload_string();

  if (payload == "HELLO") {

    needPubConfig = true;

  } else {

    needControlGPIO = true;

  }

}

void setup() {

  initVar();

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, 1);  // initial LED - OFF

  stat[1] = setStatus(analogRead(A0));

  // Setup console
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println();
  Serial.println("IoT Manager client started.");
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
        success = client.connect( MQTT::Connect( deviceId ).set_auth(mqttuser, mqttpass) );
      } else {
        success = client.connect( deviceId );
      }
      if (success) {

        Serial.println("Connect to MQTT server: Success");

        pubConfig();

        client.set_callback(onMessageArrived);

        client.subscribe(prefix);                                   // for receiving HELLO messages from IoT Manager
        client.subscribe(prefix + "/" + deviceId + "/+/control");   // for receiving GPIO control messages for ALL widgets

        Serial.println("Subscribe: Success");

      } else {

        Serial.println("Connect to MQTT server: FAIL");
        delay(1000);
      }
    }

    if (client.connected()) {

      if (needPubConfig) {
        pubConfig();
      }

      if (needControlGPIO) {
        controlGPIO();
      }

      timeNow = millis();

      if (timeNow - timer1s > 1000) { // every 1 sec

        Serial.print("."); // print activity
        timer1s = timeNow;

      }

      if (timeNow - timer10s > 10000) { // every 10 sec

        getSensorData();
        timer10s = timeNow;

      }

      client.loop();

    }
  }
  yield();
}
