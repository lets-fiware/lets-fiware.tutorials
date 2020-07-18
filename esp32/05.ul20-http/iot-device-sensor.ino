/**
 * iot-device-sensor.ino
 *
 *  IoT Device for Ultralight 2.0
 *
 *  Copyright 2018 Kazuhito Suda
 *
 *  Created on: December 8, 2018
 *
 */

#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h> // Adafruit Unified Sensor
#include <Adafruit_BME280.h> // Adafruit BME280 libary

#define SEALEVELPRESSURE_HPA (1013.25)
#define I2C_SDA 4 // BME280 SDA <---> ESP32 GPIO4
#define I2C_SCL 5 // BME280 SCL <---> ESP32 GPIO5
Adafruit_BME280 bme; // I2C

#define JST (3600*9)

const char essid[] = "your essid";
const char passphrase[] = "your passphrase";

const char iotagent[] = "http://192.168.11.1:7896/iot/d";
const char key[] = "x3knfo18xf28ekqbs93zmroz9f";
const char device[] = "sensor001";

void setup() {
  Serial.begin(115200);
  Serial.println("Start");

  connectWifi();
  configTime(JST, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");
  setupBME280();
}

void loop() {
  HTTPClient http;
  char url[128];
  char postData[128];
  time_t t = time(NULL);
  struct tm *tm = localtime(&t);

  sprintf(url, "%s?k=%s&i=%s", iotagent, key, device);
  http.begin(url);

  sprintf(postData, "d|%04d-%02d-%02dT%02d:%02d:%02d+0900|t|%2.2f|h|%2.2f|p|%4.2f",
    tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
    tm->tm_hour, tm->tm_min, tm->tm_sec,
    bme.readTemperature(),
    bme.readHumidity(),
    bme.readPressure() / 100.0F);

  Serial.printf("POST %s\n", url);
  Serial.println(postData);

  int httpCode = http.POST(postData);

  if(httpCode > 0)
    Serial.printf("httpCode: %d\n", httpCode);
  else
    Serial.printf("POST failed, error: %s\n", http.errorToString(httpCode).c_str());

  http.end();

  delay(5000);
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
