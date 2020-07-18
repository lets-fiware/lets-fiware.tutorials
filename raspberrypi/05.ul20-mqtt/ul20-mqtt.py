#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import os
import signal
import datetime
import time
import paho.mqtt.client as mqtt
from bme280 import BME280

try:
  from smbus2 import SMBus
except ImportError:
  from smbus import SMBus

EXIT = False

def handler(signum, frame):
  global EXIT
  EXIT = True

def on_connect(client, userdata, flag, response):
  print('Connected: {0}'.format(response))

def on_message(client, userdata, message):
  print(message.msg+ ' ' + str(message.payload,'utf-8'))

def on_publish(client, userdata, mid):
  print("publish: {0}".format(mid))

def main():
  signal.signal(signal.SIGTERM, handler)
  signal.signal(signal.SIGINT, handler)

  host = os.environ.get('MQTT_IP', '192.168.11.1')
  port = os.environ.get('MQTT_PORT', 1883)
  key = os.environ.get('IOTA_KEY', '8f9z57ahxmtzx21oczr5vaabot')
  device = os.environ.get('DEVICE_ID', 'sensor005')
  debug = not not os.environ.get('DEBUG_FLAG', '')
  topic = '/{}/{}/attrs'.format(key, device)

  if (debug):
    print(host)
    print(key)
    print(device)
    print(topic)

  bus = SMBus(1)
  bme280 = BME280(i2c_dev=bus)

  client = mqtt.Client()
  client.on_connect = on_connect
  client.on_message = on_message
  client.on_publish= on_publish

  client.connect(host, port, 60) 

  while True:
    date = datetime.datetime.utcnow().isoformat(timespec='seconds')
    temperature = bme280.get_temperature()
    pressure = bme280.get_pressure()
    humidity = bme280.get_humidity()
    measures = 'd|{}+0900|t|{:05.2f}|h|{:05.2f}|p|{:05.2f}'.format(date, temperature, humidity, pressure)
    if (debug):
      print(measures)
  
    client.publish(topic, measures)
    time.sleep(1)

    if (EXIT):
      break

if __name__ == '__main__':
  main()