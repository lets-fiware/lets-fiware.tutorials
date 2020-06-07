#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import os
import sys
import signal
import json
import paho.mqtt.client as mqtt
import ST7735
from PIL import Image, ImageDraw

def handler(signum, frame):
  sys.exit(0)

def on_connect(client, userdata, flags, response_code):
  print('Connected')
  topic = '/{}/{}/cmd'.format(key, device)
  client.subscribe(topic)

def on_message(client, userdata, message):
    print(message.topic + ' ' + str(message.payload,'utf-8'))
    cmd = json.loads(str(message.payload,'utf-8'))
    
    if (debug):
      print(cmd)

    if cmd.get('on') != None:
      cmd['on'] = 'OK'
      disp.display(fiware_img)

    if cmd.get('off') != None:
      cmd['off'] = 'OK'
      disp.display(blank_img)

    topic = '/json/{}/{}/cmdexe'.format(key, device)
    client.publish(topic, json.dumps(cmd))

if __name__ == '__main__':
  signal.signal(signal.SIGTERM, handler)
  signal.signal(signal.SIGINT, handler)

  print('Start Lcd device server')

  host = os.environ.get('MQTT_IP', '192.168.11.1')
  port = os.environ.get('MQTT_PORT', 1883)
  key = os.environ.get('IOTA_KEY', '8f9z57ahxmtzx21oczr5vaabot')
  device = os.environ.get('DEVICE_ID', 'lcd002')
  debug = not not os.environ.get('DEBUG_FLAG', '')

  # Create LCD class instance.
  disp = ST7735.ST7735(
      port=0,
      cs=1,
      dc=9,
      backlight=12,
      rotation=270,
      spi_speed_hz=10000000
  )

  # Initialize  LCD
  disp.begin()

  fiware_img = Image.open('./fiware.png')
  blank_img = Image.new('RGB', (disp.width, disp.height), color=(0, 0, 0))

  # Initialize MQTT
  client = mqtt.Client()

  client.on_connect = on_connect
  client.on_message = on_message

  client.connect(host, port, 60) 

  client.loop_forever()
