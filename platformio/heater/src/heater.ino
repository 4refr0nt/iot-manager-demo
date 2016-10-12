/*

  IoT Manager https://play.google.com/store/apps/details?id=ru.esp8266.iotmanager

  Tested with Platformio

  PubSubClient library v 1.91.1 https://github.com/Imroy/pubsubclient
  (use commaind: "platformio lib install https://github.com/Imroy/pubsubclient.git")

  sketch version : 1.0
  IoT Manager    : 1.5.3 and above

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>

extern "C" {
#include "user_interface.h"
}

// ADC_MODE(ADC_VCC);

#include "line01_00.h"
#include "line02_01.h"
#include "line02_02.h"
#include "line02_03.h"
#include "line03_04.h"
#include "line04_05.h"
#include "line04_06.h"
#include "line04_07.h"
#include "line05_08.h"
#include "line06_09.h"

const char *ssid =  "IoT";            // cannot be longer than 32 characters!
const char *pass =  "12345678";       // WiFi password

String prefix   = "/IoTmanager";      // global prefix for all topics - must be some as mobile device
String deviceId = "heater";

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

#define ONE_WIRE_BUS               5   // куда подключен DS18B20 - GPIO5
#define HEATER_PIN                 0   // куда подключен котел   - GPIO0

#define WIFI_INTERVAL      60 * 1000   // пытаться подключиться к WiFi каждые 60 сек
#define MQTT_INTERVAL      60 * 1000   // пытаться подключиться к MQTT каждые 60 сек
#define TEMP_INTERVAL          10000   // (10 сек - для режима отладки, 30 сек - для эксплуатации) - через какие промежутки времени производить измерения температуры
#define HELLO_INTERVAL 5 * 60 * 1000   // сколько времени посылать статусы после HELLO - 5 минут. Дожно быть больше чем 2*TEMP_INTERVAL
#define APPLY_TO_HEATER     5 * 1000   // любое изменение параметров повлияет на котел только через 5 секунд (должно быть всегда меньше TEMP_INTERVAL)
#define MEM_MAGIC                  0   // 4 bytes
#define MEM_TEMP_ON                4   // 1 byte
#define MEM_TEMP_OFF               5   // 1 byte
#define MIN_HYST                   2

#define ON                         0   // у меня на GPIO0 светодиод, он включен при низком уровне
#define OFF                        1

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

struct data_t {
  char magic[4];
  byte heaterState;
  byte tempOn;
  byte tempOff;
  byte reserved; // align to 8 bytes
} data;

int newtime, oldtime, wifiTime, helloTime, heaterTime, MQTTtime, error;
float tempC;
bool needPubConfig, needPubStatus, needControl, needParse, needHeater;
byte minHysteresis;

String t,p;

String errorMsg;

String TEMP_OFF_DEC_TOPIC        = "/IoTmanager/heater/widget01";
String TEMP_OFF_TOPIC            = "/IoTmanager/heater/widget02";
String TEMP_OFF_INC_TOPIC        = "/IoTmanager/heater/widget03";
String TEMP_TOPIC                = "/IoTmanager/heater/widget04";
String TEMP_ON_DEC_TOPIC         = "/IoTmanager/heater/widget05";
String TEMP_ON_TOPIC             = "/IoTmanager/heater/widget06";
String TEMP_ON_INC_TOPIC         = "/IoTmanager/heater/widget07";
String HEATER_TOPIC              = "/IoTmanager/heater/widget08";
String ERROR_TOPIC               = "/IoTmanager/heater/widget09";

void initVar() {

  minHysteresis = MIN_HYST;

  deviceId = "heater";

  error = 0;
  errorMsg = "";

  // загружаем температуру tempOn tempOff
  dataLoad();

  data.heaterState = OFF;

  needPubConfig = true;
  needPubStatus = true;
  needHeater    = true;
  needControl   = false;

  wifiTime = - WIFI_INTERVAL;
  MQTTtime = - MQTT_INTERVAL;
}

void restart() {
   ESP.restart();
}
void pubTempOn() {
  pubStatus(TEMP_ON_TOPIC, data.tempOn);
}
void pubTempOff() {
  pubStatus(TEMP_OFF_TOPIC, data.tempOff);
}
void pubTemp() {
  needPubStatus = false;

  pubStatus(TEMP_TOPIC, tempC);

  pubStatus(ERROR_TOPIC, errorMsg);
  // reset error status
  error = 0;
  errorMsg = "";
}

void pubStatus(String t, float temp) {
  pubStatus(t, String(temp));
}
void pubStatus(String t, byte temp) {
  pubStatus(t, String(temp));
}
void pubStatus(String t, String temp) {

    String p = "{\"status\":\"" + temp + "\"}";

    pubStatusJSON(t,p);

}
void pubStatus(String t, String s, String e) {

    String p = "{\"status\":\"" + s + "\", \"class3\":\"" + e +"\"}";

    pubStatusJSON(t,p);

}
void pubStatusJSON(String t, String p) {
  if (client.publish(t + "/status", p)) {
     Serial.println("Publish new status to " + t + "/status" + ", value: " + p);
  } else {
     Serial.println("Publish new status to " + t + "/status" + " FAIL!");
     client.disconnect();
  }
}
void pub(String t, const char p[]) {
  bool success = client.publish(MQTT::Publish(t, p).set_qos(1));
  if (success) {
    Serial.println("Publish widget config: Success.");
  } else {
    Serial.println("Publish FAIL!");
  }
}
void pubConfig() {
  needPubConfig = false;

  pub(prefix + "/" + deviceId + "/config", line01_00_json);
  pub(prefix + "/" + deviceId + "/config", line02_01_json);
  pub(prefix + "/" + deviceId + "/config", line02_02_json);
  pub(prefix + "/" + deviceId + "/config", line02_03_json);
  pub(prefix + "/" + deviceId + "/config", line03_04_json);
  pub(prefix + "/" + deviceId + "/config", line04_05_json);
  pub(prefix + "/" + deviceId + "/config", line04_06_json);
  pub(prefix + "/" + deviceId + "/config", line04_07_json);
  pub(prefix + "/" + deviceId + "/config", line05_08_json);
  pub(prefix + "/" + deviceId + "/config", line06_09_json);

  needPubStatus = true;
}
void parseMessage() {

  needParse = false;

  Serial.print("New message from: ");
  Serial.print(t);
  Serial.print(" => ");
  Serial.println(p);

  if ( p == "HELLO" ) {    // handshaking
     helloTime = millis(); // засекаем время прихода HELLO
     needPubConfig = true;
  } else {
     needControl = true;
  }

}
void onMessageReceived(const MQTT::Publish& sub) {

  t = sub.topic();
  p = sub.payload_string();
  needParse = true;

}

void checkHeater() {

  if ((tempC >= data.tempOff) && (data.heaterState == ON)) {
    needHeater = true;
    data.heaterState = OFF;
    heaterTime = millis();
    Serial.println();
    Serial.println("HEATER schedulled new state: OFF");
  } else if ((tempC <= data.tempOn) && (data.heaterState == OFF)) {
    needHeater = true;
    data.heaterState = ON;
    heaterTime = millis();
    Serial.println();
    Serial.println("HEATER schedulled new state: ON");
  } else {
    Serial.print(" HEATER state: ");
    Serial.print(data.heaterState);
    Serial.print(" tempOff: ");
    Serial.print(data.tempOff);
    Serial.print(" tempOn: ");
    Serial.println(data.tempOn);
  }

}

void getTemp(){

  Serial.print("Requesting temp ...");
  sensors.requestTemperatures();
  Serial.print(" done. ");

  tempC = sensors.getTempCByIndex(0);

  if (isnan(tempC) || tempC == -127.0) {
    error = 2;
    errorMsg = "Temp sensor error";
    Serial.println(errorMsg);
    // restart(); ? по идее нет смысла продолжать, если датчик отвалился
    return;
  }

  Serial.print("Temp value = ");
  Serial.print(tempC);

  checkHeater();

  // float vcc = 0.00f;
  // vcc = ESP.getVcc();
  // Serial.print("VCC = ");
  // Serial.println(vcc/1024.00f);

}

void control() {
  needControl = false;

  if (t == TEMP_OFF_DEC_TOPIC + "/control") {
    if (data.tempOff > (data.tempOn + minHysteresis)) {
      --data.tempOff;
      dataSave();
      Serial.print("TempOff : ");
      Serial.println(data.tempOff);
      pubTempOff();
    }
  }
  if (t == TEMP_OFF_INC_TOPIC + "/control") {
    ++data.tempOff;
    dataSave();
    Serial.print("TempOff : ");
    Serial.println(data.tempOff);
    pubTempOff();
  }
  if (t == TEMP_ON_DEC_TOPIC + "/control") {
    --data.tempOn;
    dataSave();
    Serial.print("TempOn : ");
    Serial.println(data.tempOn);
    pubTempOn();
  }
  if (t == TEMP_ON_INC_TOPIC + "/control") {
    if (data.tempOn < (data.tempOff - minHysteresis)) {
      ++data.tempOn;
      dataSave();
      Serial.print("TempOn : ");
      Serial.println(data.tempOn);
      pubTempOn();
    }
  }
}

void dataLoad() {
  int i;
  byte tmp[sizeof(data)];

  EEPROM.begin(sizeof(data));

  for (i = 0; i < sizeof(data); i++) {
      tmp[i] = EEPROM.read(i);
  }

  if (tmp[0] == 50 && tmp[1] == 12 && tmp[2] == 5 && tmp[3] == 12)   {
    // magic detected, loading
    memcpy(&data, tmp,  sizeof(data));
    Serial.println("Stored data loaded.");
  } else {
    // первый запуск
    Serial.println("Stored data not found, using defaults.");

    data.magic[0]    = 50;
    data.magic[1]    = 12;
    data.magic[2]    =  5;
    data.magic[3]    = 12;

    data.tempOn      = 18;
    data.tempOff     = 22;
    data.heaterState = OFF;

    dataSave();
  }

}
void dataSave() {
  int i;
  byte tmp[sizeof(data)];

  EEPROM.begin(sizeof(data));

  memcpy(tmp, &data, sizeof(data));

  for (i = 0; i < sizeof(data); i++)  {
    EEPROM.write(i, tmp[i]);
  }
  EEPROM.commit();

  Serial.println("Data saved.");
}

void heater() {
  needHeater = false;

  digitalWrite(HEATER_PIN, data.heaterState);
  Serial.print("HEATER change state to ");
  Serial.println(data.heaterState);

  pubHeater();

}

void pubHeater(){
  if (data.heaterState == ON) {
    pubStatus(HEATER_TOPIC, "Heater: ON", "balanced-bg light rounded padding text-center");
  } else {
    pubStatus(HEATER_TOPIC, "Heater: OFF", "assertive-bg light rounded padding text-center");
  }
}

void setup() {

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  Serial.begin(115200);
  delay(10);
  WiFi.printDiag(Serial);
  Serial.println();
  Serial.println();
  Serial.println("HEATER started.");
  Serial.println();

  uint32_t realSize = ESP.getFlashChipRealSize();
  uint32_t ideSize = ESP.getFlashChipSize();
  FlashMode_t ideMode = ESP.getFlashChipMode();
  Serial.printf("Flash real size: %u\n", realSize);
  Serial.printf("Flash ide  size: %u\n", ideSize);
  Serial.printf("Flash ide speed: %u\n", ESP.getFlashChipSpeed()/1000000);
  Serial.printf("Flash ide mode:  %s\n", (ideMode == FM_QIO ? "QIO" : ideMode == FM_QOUT ? "QOUT" : ideMode == FM_DIO ? "DIO" : ideMode == FM_DOUT ? "DOUT" : "UNKNOWN"));
  Serial.print("SDK version: ");
  Serial.println(system_get_sdk_version());

  Serial.println();
  initVar();


  pinMode(ONE_WIRE_BUS, INPUT_PULLUP);
  delay(200);
  sensors.begin();
  int sensorCount = sensors.getDeviceCount();
  Serial.print("Sensors counter is: ");
  Serial.println(sensorCount);

  if (sensorCount < 1) {
    error = 1;
    errorMsg = "Temp sensor not found!";
    Serial.println(errorMsg);
    // может имеет смысл перезагружаться, пока датчика нет
  }

  // report parasite power requirements
  Serial.print("Parasite power requirements is: ");
  if (sensors.isParasitePowerMode()) {
    Serial.println("ON");
  }  else {
    Serial.println("OFF");
  }

  pinMode(HEATER_PIN, OUTPUT);

  // пока не выставляем реле
  // heater();
}
void MQTTConnect() {
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }
  if (client.connected()) {
    return;
  }
      Serial.print("Connecting to MQTT server ... ");
      bool success;
      if (mqttuser.length() > 0) {
        success = client.connect( MQTT::Connect( deviceId ).set_auth(mqttuser, mqttpass) );
      } else {
        success = client.connect( deviceId );
      }
      if (success && client.connected()) {
        client.set_callback(onMessageReceived);
        Serial.println("Success");
        client.subscribe(prefix);  // for receiving HELLO messages and handshaking
        client.subscribe(prefix + "/" + deviceId + "/+/control"); // subscribe to all "control" messages for all widgets of this device
        needPubConfig = true;
      } else {
        Serial.println("FAIL");
      }
  return;
}
void WiFiConnect() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting via WiFi to ");
    Serial.print(ssid);
    Serial.print("...");

    WiFi.begin(ssid, pass);

    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println(" FAIL!");
      return;
    }

    Serial.println(" Success.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
}
void loop() {

  newtime = millis();

  if (newtime - wifiTime > WIFI_INTERVAL) {
    WiFiConnect();
    yield();
    wifiTime = millis();
  }

  if ((WiFi.status() == WL_CONNECTED) && ((millis() - MQTTtime) > MQTT_INTERVAL)) {
    MQTTConnect();
    yield();
    MQTTtime = millis();
  }

  if (newtime - oldtime > TEMP_INTERVAL) {
    // измеряем температуру каждые TEMP_INTERVAL/1000 секунд даже если нет связи
    getTemp();
    oldtime = newtime;

    if ((newtime - helloTime) < HELLO_INTERVAL) {
      // если после HELLO прошло меньше HELLO_INTERVAL/1000 сек, то отправляем статус при каждом измерении
      needPubStatus = true;
    }
  }

 if (client.connected()) {

      if (needParse)     parseMessage();
      yield();
      if (needPubConfig) pubConfig();
      yield();
      if (needPubStatus) {
        pubTemp();
        yield();
        pubTempOff();
        yield();
        pubTempOn();
        yield();
        pubHeater();
        yield();
      }
      if (needControl)   control();
      yield();
      if (needHeater && ((newtime - heaterTime) > APPLY_TO_HEATER)) {
        heater();
      }
      client.loop();
  }
  yield();
}
