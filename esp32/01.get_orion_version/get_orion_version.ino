/**
 * get_orion_version.ino
 *
 *  get FIWARE Orion version
 *  
 *  Copyright 2018 Kazuhito Suda
 *  
 *  Created on: Nobember 11,2018
 *
 */

#include <WiFi.h>
#include <HTTPClient.h>

char essid[] = "your essid";
char passphrase[] = "your passphrase";
char orion[] = "http://192.168.1.2:1026/version";

void setup() {
  Serial.begin(115200);
  Serial.print("start");
  
  connectWifi();
 
  getOrionVersion(orion);
  
  disconnectWifi();
}

void loop() {

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
  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway IP: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("DNS: ");
  Serial.println(WiFi.dnsIP());
  Serial.print("Mac Address: ");
  Serial.println(WiFi.macAddress());
}

void disconnectWifi() {
  WiFi.disconnect();
  Serial.println("disconnected");
}

void getOrionVersion(char url[]) {
  HTTPClient http;

  http.begin(url);
  int httpCode = http.GET();

  if(httpCode > 0) {
    Serial.printf("httpCode: %d\n", httpCode);
    
    if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("POST failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}
