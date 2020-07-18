/**
 * context-consumer-01.ino
 *
 *  Context consumer
 *  
 *  Copyright 2018 Kazuhito Suda
 *  
 *  Created on: December 9, 2018
 *
 */
 
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <Wire.h>
 
char essid[] = "your essid";
char passphrase[] = "your passphrase";
char orion[] = "http://192.168.1.1:1026";
 
const char entityType[] = "WeatherObserved";
const char entityId[] = "urn:ngsi-ld:WeatherObserved:sensor001";
 
byte ST7032i = 0x3E; // LCD
 
WebServer server(80);
 
void setup() {
  Serial.begin(115200);
  Serial.println("Start");
   
  connectWifi();
 
  server.on("/", accumulator);
  server.onNotFound(handleNotFound);
  server.begin();
 
  Wire.begin();
  initLCD();
  
  createSubscription();
}
 
void loop() {
  server.handleClient();
}
 
void accumulator() {
  if (server.method() == HTTP_POST && server.args() == 1) {
    const size_t bufferSize = JSON_ARRAY_SIZE(1) + JSON_OBJECT_SIZE(0) + JSON_OBJECT_SIZE(2) + 2*JSON_OBJECT_SIZE(3) + 170;
    StaticJsonBuffer <bufferSize> jsonBuffer;
 
    JsonObject& root = jsonBuffer.parseObject(server.arg(0));
 
    const char* subscriptionId = root["subscriptionId"];
 
    JsonObject& data0 = root["data"][0];
    const char* data0_id = data0["id"];
    const char* data0_type = data0["type"];
 
    JsonObject& data0_temperature = data0["temperature"];
    const char* data0_temperature_type = data0_temperature["type"];
    float data0_temperature_value = data0_temperature["value"];
 
    Serial.printf("%s %s %s %2.2f\n", subscriptionId, data0_id, data0_type, data0_temperature_value);
 
    char buff[9];
    sprintf(buff, "%2.2f", data0_temperature_value);
    printLCD(buff);
    
    server.send(200);
  } else {
    sendErrorMessage(400, "BadRequest", "service not found"); 
  }
}
 
void createSubscription() {
  HTTPClient http;
  char url[128];
 
  sprintf(url, "%s/v2/subscriptions", orion);
  http.begin(url);
  
  http.addHeader("Content-Type", "application/json");
 
  // payload
  const size_t bufferSize = 3*JSON_ARRAY_SIZE(1) + 2*JSON_OBJECT_SIZE(1) + 3*JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(5);
  StaticJsonBuffer <bufferSize> jsonBuffer;
 
  JsonObject& root = jsonBuffer.createObject();
  root["description"] = "A subscription to get info about sensor001";
 
  JsonObject& subject = root.createNestedObject("subject");
 
  JsonArray& subject_entities = subject.createNestedArray("entities");
 
  JsonObject& subject_entities_0 = subject_entities.createNestedObject();
  subject_entities_0["id"] = entityId;
  subject_entities_0["type"] = entityType;
 
  JsonObject& subject_condition = subject.createNestedObject("condition");
  JsonArray& subject_condition_attrs = subject_condition.createNestedArray("attrs");
  subject_condition_attrs.add("temperature");
 
  JsonObject& notification = root.createNestedObject("notification");
  JsonObject& notification_http = notification.createNestedObject("http");
  notification_http["url"] = "http://" + WiFi.localIP().toString() + "/";
  JsonArray& notification_attrs = notification.createNestedArray("attrs");
  notification_attrs.add("temperature");
  root["expires"] = "2040-01-01T14:00:00.00Z";
  root["throttling"] = 1;
 
  String postData = "";
  root.printTo(postData);
 
  Serial.println("Subscription : ");
  Serial.printf("POST %s\n", url);  
  Serial.println(postData);
  
  int httpCode = http.POST(postData);
 
  if(httpCode > 0)
    Serial.printf("httpCode: %d\n", httpCode);
  else
    Serial.printf("POST failed, error: %s\n", http.errorToString(httpCode).c_str());
 
  http.end();  
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
 
void initLCD() {
  byte cmds[] = {0x38, 0x39, 0x14, 0x73, 0x56, 0x6c, 0x38, 0x01, 0x0c};
  char fiware[] = "FIWARE  ";
  
  for (int i = 0; i < sizeof(cmds); i++) {
    writeCmd(cmds[i]);
  }
 
  for (int i = 0; i < sizeof(fiware); i++) {
    writeData(fiware[i]);
  }
  
  writeCmd(0xc0);
  for (int i = 0; i < 8; i++) {
    writeData(' ');
  }
  
  writeCmd(0x0C); 
}
 
void printLCD(char str[]) {
  writeCmd(0xC0);
  for (int i = 0; i < strlen(str); i++) {
    writeData(str[i]);
  }  
}
 
void writeData(byte data) {
  delay(1);
  Wire.beginTransmission(ST7032i);
  Wire.write(0x40);
  Wire.write(data);
  Wire.endTransmission();
}
 
void writeCmd(byte cmd) {
  delay(30);
  Wire.beginTransmission(ST7032i);
  Wire.write(0x00);
  Wire.write(cmd);
  Wire.endTransmission();
}
