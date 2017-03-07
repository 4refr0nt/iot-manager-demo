/*
    Push notification for IoT Manager

    v 1.0

    Test on Arduino IDE 1.8.1
    
    Based on "HTTP over TLS (HTTPS) example" sketch

    This example demonstrates how to use
    WiFiClientSecure class to access HTTPS API.
    We fetch and display the status of
    esp8266/Arduino project continuous integration
    build.

    Created by Ivan Grokhotkov, 2015.
    This example is in public domain.
*/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

/* user parameters */

const char* ssid = "IoT";
const char* password = "12345678";

// IDS - unique device id from IoT Manager application: Start IoT Manager, goto "Statistic" and press "Send ids to email"
// Also, you can get ids via MQTT after HELLO message.
// ids will be always changed after IoT Manager reinstall on mobile device

String ids = "91433959-a3f5-4cc1-aa28-22a6467b52f3"; // its not real IDS, please change

/* end of user parameters */



const char* host = "onesignal.com";
const int httpsPort = 443;
String url = "/api/v1/notifications";

void push(String msg) {
  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;

  Serial.print("PUSH: connecting to ");
  Serial.println(host);

  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
  Serial.println("PUSH: try to send push notification...");

  // please, do not change app_id - its IoT Manager id at onesignal.com
  // more info at https://documentation.onesignal.com/v3.0/reference#create-notification
  
  String data = "{\"app_id\": \"8871958c-5f52-11e5-8f7a-c36f5770ade9\",\"include_player_ids\":[\"" + ids + "\"],\"android_group\":\"IoT Manager\",\"contents\": {\"en\": \"" + msg + "\"}}";

  Serial.print("PUSH: requesting URL: ");
  Serial.println(url);
  
  client.println(String("POST ") + url + " HTTP/1.1");
  client.print("Host:");
  client.println(host);
  client.println("User-Agent: esp8266.Arduino.IoTmanager");
  client.print("Content-Length: ");
  client.println(data.length());
  client.println("Content-Type: application/json");
  client.println("Connection: close");
  client.println();
  client.println(data);

  Serial.println("PUSH: done. Restart esp8266 for push again.");
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Push notification for IoT Manager");
  Serial.println();
  Serial.print("PUSH: connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("PUSH: WiFi connected");

  push("test message");
}

void loop() {
}
