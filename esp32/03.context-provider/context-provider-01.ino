/**
 * context-provider-01.ino
 *
 *  Context Provider
 *  
 *  Copyright 2018 Kazuhito Suda
 *  
 *  Created on: November 18, 2018
 *
 */

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h> // Adafruit Unified Sensor
#include <Adafruit_BME280.h> // Adafruit BME280 libary
#include <ArduinoJson.h> // ArduinoJson 5.13.3

#define SEALEVELPRESSURE_HPA (1013.25)
#define I2C_SDA 4 // BME280 SDA <---> ESP32 GPIO4 
#define I2C_SCL 5 // BME280 SCL <---> ESP32 GPIO5
Adafruit_BME280 bme; // I2C

char essid[] = "your essid";
char passphrase[] = "your passphrase";

const char entityType[] = "WeatherObserved";
const char entityId[] = "urn:ngsi-ld:WeatherObserved:sensor002";

WebServer server(80);

void setup() {
  Serial.begin(115200);
  Serial.println("Start");
   
  setupBME280();
  connectWifi();
 
  server.on("/v1/weatherObserved/queryContext", queryContext);  
  server.onNotFound(handleNotFound);
  server.begin();
}

void loop() {
  server.handleClient();
}

void queryContext() {
  if (server.method() != HTTP_POST && server.args() != 1) {
    sendErrorMessage(400, "BadRequest", "service not found");
    return;
  }

  String message = "";
  for (uint8_t i=0; i<server.args(); i++){
    message += server.argName(i) + ": " + server.arg(i) + "\n";
  }
  Serial.print(message); 

  StaticJsonBuffer<500> jsonBufferPost;

  JsonObject& rootPost = jsonBufferPost.parseObject(server.arg(0));

  if (!rootPost.success()) {
    sendErrorMessage(400, "BadRequest", "post data is incorrect");
    return;
  }

  JsonObject& entities = rootPost["entities"][0];

  auto type = entities["type"].as<char*>();
  if (type == NULL || strcmp(type, entityType) != 0) {
    sendErrorMessage(404, "Not found", "entity type");
    return;
  }

  auto id = entities["id"].as<char*>();
  if (id == NULL || strcmp(id, entityId) != 0) {
    sendErrorMessage(404, "Not found", "entity id");
    return;
  }

  const size_t bufferSize = JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(3) + JSON_OBJECT_SIZE(1) + 2*JSON_OBJECT_SIZE(2) + 3*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + 50;
  StaticJsonBuffer<bufferSize> jsonBuffer;

  JsonObject& root = jsonBuffer.createObject();

  JsonArray& contextResponses = root.createNestedArray("contextResponses");

  JsonObject& contextResponses_0 = contextResponses.createNestedObject();

  JsonObject& contextResponses_0_contextElement = contextResponses_0.createNestedObject("contextElement");

  JsonArray& contextResponses_0_contextElement_attributes = contextResponses_0_contextElement.createNestedArray("attributes");

  JsonArray& attributes = rootPost["attributes"];
  for (int i = 0; i < attributes.size(); i++ ) {
    auto attr = attributes[i].as<char*>();
    if (attr == NULL || !addAttributes(contextResponses_0_contextElement_attributes, attr)) {
      sendErrorMessage(404, "Not found", "attribute");
      return;
    }
  }
  
  contextResponses_0_contextElement["id"] = entityId;
  contextResponses_0_contextElement["isPattern"] = "false";
  contextResponses_0_contextElement["type"] = entityType;

  JsonObject& statusCode = contextResponses_0.createNestedObject("statusCode");
  statusCode["code"] = "200";
  statusCode["reasonPhrase"] = "OK";

  String postData = "";
  root.printTo(postData);
  Serial.println(postData);
  
  server.send(200, "application/json", postData);
}

bool addAttributes(JsonArray& jsonArray, const char *attrName) {

  if (!strcmp(attrName, "temperature")) {
    JsonObject& attr = jsonArray.createNestedObject();
    attr["name"] = "temperature";
    attr["type"] = "Number";
    attr["value"] = bme.readTemperature();

  } else if (!strcmp(attrName, "relativeHumidity")) {
    JsonObject& attr = jsonArray.createNestedObject();
    attr["name"] = "relativeHumidity";
    attr["type"] = "Number";
    attr["value"] = bme.readHumidity();

  } else if (!strcmp(attrName, "atmosphericPressure")) {
    JsonObject& attr = jsonArray.createNestedObject();
    attr["name"] = "atmosphericPressure";
    attr["type"] = "Number";
    attr["value"] = bme.readPressure() / 100.0F;

  } else {
    return false;

  }
  return true;
}

void handleNotFound() {
  sendErrorMessage(400, "BadRequest", "service not found");
}

void sendErrorMessage(int status, String err, String description) {
  String message = "{\"error\":\"" + err + "\",\"description\":\"" + description + "\"}";
  server.send(status, "application/json", message);
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
  Serial.print("IP address: ");
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
