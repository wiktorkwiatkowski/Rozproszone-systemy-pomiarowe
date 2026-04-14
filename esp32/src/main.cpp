#include "secrets.h"
#include "sensorsim.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>
WiFiClient espClient;
PubSubClient mqttClient(espClient);
String deviceId;
String topic;
struct seq_counter
{
    int temp = 0;
    int humi = 0;
    int press = 0;
};

struct seq_counter seq_c;

String generateDeviceIdFromEfuse() {
  uint64_t chipId = ESP.getEfuseMac();
  char id[32];
  snprintf(id, sizeof(id), "esp32-%04X%08X", (uint16_t)(chipId >> 32),
           (uint32_t)chipId);
  return String(id);
}

void connectWiFi() {
  Serial.print("Laczenie z Wi-Fi: ");
  Serial.println(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Polaczono z Wi-Fi");
  Serial.print("Adres IP: ");
  Serial.println(WiFi.localIP());
}

void connectMQTT() {
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  while (!mqttClient.connected()) {
    Serial.print("Laczenie z MQTT...");
    if (mqttClient.connect(deviceId.c_str())) {
      Serial.println("OK");
    } else {
      Serial.print("blad, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" - ponowna proba za 2 s");
      delay(2000);
    }
  }
}

long long getTimestampMs() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return ((long long)tv.tv_sec * 1000LL) + (tv.tv_usec / 1000);
}

void publishMeasurement(String name, float data, String unit, int seq) {
  JsonDocument doc;
  doc["device_id"] = deviceId;
  doc["sensor"] = name;
  doc["value"] = data;
  doc["unit"] = unit;
  doc["ts_ms"] = getTimestampMs();
  doc["seq"] = seq;
  char payload[256];
  serializeJson(doc, payload);
  String full_topic = topic + name;
  mqttClient.publish(full_topic.c_str(), payload);
  Serial.print("Publikacja na topic: ");
  Serial.println(full_topic);
  Serial.println(payload);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  deviceId = generateDeviceIdFromEfuse();
  topic = "lab/" + String(MQTT_GROUP) + "/" + deviceId + "/";
  Serial.print("Device ID: ");
  Serial.println(deviceId);
  connectWiFi();
  connectMQTT();

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    Serial.println("Oczekiwanie na synchronizacje czasu...");
    delay(500);
  }
  Serial.println("Czas zsynchronizowany.");
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }
  if (!mqttClient.connected()) {
    connectMQTT();
  }
  float temp = sinsim(25.0, 5.0, 60.0);
  float humidity = sinsim(55.0, 15.0, 90.0);
  float pressure = sinsim(1010.0, 10.0, 120.0);

  publishMeasurement("temperature", temp, "C", seq_c.temp++);
  publishMeasurement("humidity", humidity, "%", seq_c.humi++);
  publishMeasurement("pressure", pressure, "hPa", seq_c.press++);

  mqttClient.loop();
  delay(5000);
}