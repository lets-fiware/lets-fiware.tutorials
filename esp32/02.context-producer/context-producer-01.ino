/**
 * context-producer-01.ino
 *
 *  Context Producer
 *  
 *  Copyright 2018 Kazuhito Suda
 *  
 *  Created on: November 18, 2018
 *
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h> // Adafruit Unified Sensor
#include <Adafruit_BME280.h> // Adafruit BME280 libary
#include <ArduinoJson.h> // ArduinoJson 5.13.3

#define SEALEVELPRESSURE_HPA (1013.25)
#define I2C_SDA 4 // BME280 SDA <---> ESP32 GPIO4 
#define I2C_SCL 5 // BME280 SCL <---> ESP32 GPIO5
Adafruit_BME280 bme; // I2C

#define JST (3600*9)

const char essid[] = "your essid";
const char passphrase[] = "your passphrase";
char orion[] = "http://192.168.1.2:1026";

const char entityType[] = "WeatherObserved";
const char entityId[] = "urn:ngsi-ld:WeatherObserved:sensor001";
const float longitude = 139.767052;
const float latitude = 35.681167;

void setup() {
  Serial.begin(115200);
  Serial.println("Start");
  
  connectWifi();
  configTime(JST, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");
  setupBME280();
     
  deleteEntity(orion, entityId);
  createEntity(orion, entityType, entityId);
}

void loop() {
  delay(5000);
  
  updateAttrs(orion, entityId);
}

void connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(essid, passphrase);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }

  Serial.println("");
  Serial.println("Connected");
  Serial.print("IP address : "); 
  Serial.println(WiFi.localIP());
}

void setupBME280() {
    Wire.begin(I2C_SDA, I2C_SCL);
    bool status = bme.begin();  
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }
}

void deleteEntity(const char orion[], const char id[]) {
  HTTPClient http;
  int httpCode;
  char url[128];

  sprintf(url, "%s/v2/entities/%s", orion, id);

  Serial.printf("GET %s\n", url);  
  http.begin(url);
  httpCode = http.GET();

  if (httpCode > 0)
    Serial.printf("httpCode: %d\n", httpCode);
  else
    Serial.printf("GET failed, error: %s\n", http.errorToString(httpCode).c_str());
      
  if (httpCode == 200) {
    http.end();
    http.begin(url);
  
    Serial.printf("DELETE %s\n", url);  
    httpCode = http.sendRequest("DELETE");

    if (httpCode > 0)
      Serial.printf("httpCode: %d\n", httpCode);
    else
      Serial.printf("DELETE failed, error: %s\n", http.errorToString(httpCode).c_str());
  } 
  http.end();
}   

void createEntity(const char orion[], const char type[], const char id[]) {
  HTTPClient http;
  char url[128];
  
  sprintf(url, "%s/v2/entities", orion);
  http.begin(url);
  
  http.addHeader("Content-Type", "application/json");

  const size_t bufferSize = JSON_ARRAY_SIZE(2) + 6*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(7) + 50;
  StaticJsonBuffer <bufferSize> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();
  root["type"] = type;
  root["id"] = id;

  // dateObserved
  char buf[30];
  JsonObject& dateObserved = root.createNestedObject("dateObserved");
  dateObserved["type"] = "DateTime";
  dateObserved["value"] = getDateTime(buf);

  addAttribute(root);

  // location
  JsonObject& location = root.createNestedObject("location");
  location["type"] = "geo:json";

  JsonObject& location_value = location.createNestedObject("value");
  location_value["type"] = "Point";

  JsonArray& location_value_coordinates = location_value.createNestedArray("coordinates");
  location_value_coordinates.add(longitude);
  location_value_coordinates.add(latitude);

  String postData = "";
  root.printTo(postData);

  Serial.printf("POST %s\n", url);  
  Serial.println(postData);
  
  int httpCode = http.POST(postData);

  if(httpCode > 0)
    Serial.printf("httpCode: %d\n", httpCode);
  else
    Serial.printf("POST failed, error: %s\n", http.errorToString(httpCode).c_str());

  http.end();
}

void updateAttrs(const char orion[], const char id[]) {
  HTTPClient http;

  char url[128];
  sprintf(url, "%s/v2/entities/%s/attrs", orion, id);
  http.begin(url);
  
  http.addHeader("Content-Type", "application/json");

  const size_t bufferSize = 4*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + 50;
  StaticJsonBuffer<bufferSize> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  // dateObserved
  char buf[30];
  JsonObject& dateObserved = root.createNestedObject("dateObserved");
  dateObserved["type"] = "DateTime";
  dateObserved["value"] = getDateTime(buf);

  addAttribute(root);
 
  String postData = "";
  root.printTo(postData);

  Serial.printf("PATCH %s\n", url);
  Serial.println(postData);
    
  int httpCode = http.sendRequest("PATCH", postData);

  if(httpCode > 0)
    Serial.printf("httpCode: %d\n", httpCode);
  else
    Serial.printf("PATCH failed, error: %s\n", http.errorToString(httpCode).c_str());

  http.end();
}

void addAttribute(JsonObject& root) {
  // temperature
  JsonObject& temperature = root.createNestedObject("temperature");
  temperature["type"] = "Number";
  temperature["value"] = bme.readTemperature();

  // relativeHumidity
  JsonObject& relativeHumidity = root.createNestedObject("relativeHumidity");
  relativeHumidity["type"] = "Number";
  relativeHumidity["value"] = bme.readHumidity();

  // atmosphericPressure
  JsonObject& atmosphericPressure = root.createNestedObject("atmosphericPressure");
  atmosphericPressure["type"] = "Number";
  atmosphericPressure["value"] = bme.readPressure() / 100.0F;
}

char* getDateTime(char* buf) {
  time_t t = time(NULL);
  struct tm *tm = localtime(&t);

  sprintf(buf, "%04d-%02d-%02dT%02d:%02d:%02d+0900", 
           tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
           tm->tm_hour, tm->tm_min, tm->tm_sec);

  return buf;
}
