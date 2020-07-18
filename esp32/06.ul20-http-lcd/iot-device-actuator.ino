/**
 * iot-device-actuator.ino
 *
 *  Iot Device Actuator
 *
 *  Copyright 2018 Kazuhito Suda
 *
 *  Created on: December 8, 2018
 *
 */

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

char essid[] = "your essid";
char passphrase[] = "your passphrase";

const char url[] = "/iot/led001";
const char device[] = "led001";

const int led = 19;

WebServer server(80);

void setup() {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  Serial.println("Start");

  connectWifi();

  server.on(url, execCmd);
  server.onNotFound(handleNotFound);
  server.begin();
}

void loop() {
  server.handleClient();
}

void execCmd() {
  if (server.method() == HTTP_POST && server.args() == 1) {
    int status = 200;
    int atmark;
    int pipe;
    String postData = server.arg(0);
    Serial.println(postData);

    if (postData.startsWith(device)
       && (atmark = postData.indexOf('@')) != -1
       && (pipe = postData.indexOf('|', atmark+1)) != -1) {
       String cmd = postData.substring(atmark+1, pipe);
       if (!cmd.compareTo("on")) {
         digitalWrite(led, 1);
         postData += cmd + " OK";
       } else if (!cmd.compareTo("off")) {
         digitalWrite(led, 0);
         postData += cmd + " OK";
       } else {
         postData += cmd + " ERROR";
         status = 400;
       }
    } else {
       postData += "ERROR";
       status = 400;
    }
    Serial.println(postData);
    server.send(status, "text/html", postData);
  } else {
    sendErrorMessage(400, "BadRequest", "service not found");
  }
}

void handleNotFound() {
  sendErrorMessage(400, "BadRequest", "service not found");
}

void sendErrorMessage(int status, String err, String description) {
  digitalWrite(led, 1);
  String message = "{\"error\":\"" + err + "\",\"description\":\"" + description + "\"}";
  server.send(status, "application/json", message);
  digitalWrite(led, 0);
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
