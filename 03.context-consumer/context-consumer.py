#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import os
import sys
import signal
from http.server import BaseHTTPRequestHandler, HTTPServer
from urllib.parse import parse_qs, urlparse
import requests
import json
import ST7735
from PIL import Image, ImageDraw, ImageFont
from fonts.ttf import RobotoMedium as UserFont

class ContextConsumerHandler(BaseHTTPRequestHandler):
  def do_POST(self):
    content_length = int(self.headers['content-length'])
    payload = json.loads(self.rfile.read(content_length).decode('utf-8'))

    if (debug):
      print(payload)

    temp = 'temp:{}'.format(payload['data'][0]['temperature']['value'])
    size_x, size_y = draw.textsize(temp, font)
    
    # Calculate text position
    x = (WIDTH - size_x) / 2
    y = (HEIGHT / 2) - (size_y / 2)
    
    # Draw background rectangle and write text.
    draw.rectangle((0, 0, 160, 80), back_colour)
    draw.text((x, y), temp, font=font, fill=text_colour)
    disp.display(img) 

    self.send_response(200)
    self.end_headers()
        
def handler(signum, frame):
  sys.exit(0)

if __name__ == '__main__':
  signal.signal(signal.SIGTERM, handler)
  signal.signal(signal.SIGINT, handler)

  fiware_service = ''
  fiware_servicepath = '/device'
  orion_url = os.environ.get('ORION_URL', 'http://192.168.1.1:1026')
  context_consumer_url = os.environ.get('CONTEXT_CONSUMER_URL', 'http://192.168.1.2:8080/')
  device_id = 'urn:ngsi-ld:WeatherObserved:' + os.environ.get('DEVCIE_ID', 'sensor001')
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

  # Width and height to calculate text position.
  WIDTH = disp.width
  HEIGHT = disp.height

  # New canvas to draw on.
  img = Image.new('RGB', (WIDTH, HEIGHT), color=(0, 0, 0))
  draw = ImageDraw.Draw(img)

  # Text settings.
  font_size = 25
  font = ImageFont.truetype(UserFont, font_size)
  text_colour = (255, 255, 255)
  back_colour = (0, 170, 170)

  # Initialize display.
  disp.begin()

  # Create Subscription
  subscription = {
    "description": "A subscription to get info about sensor001",
    "subject": {
      "entities": [
        {
          "id": device_id,
          "type": "WeatherObserved"
        }
      ],
      "condition": {
        "attrs": [
          "temperature"
        ]
      }
    },
    "notification": {
      "http": {
        "url": context_consumer_url
      },
      "attrs": [
        "temperature"
      ]
    }
  }

  if (debug):
    print(subscription)

  headers = {
    'Fiware-Service': fiware_service,
    'Fiware-ServicePath': fiware_servicepath,
    'Content-Type': 'application/json; charset=utf-8'
  }
  url = orion_url + '/v2/subscriptions'

  r = requests.post(url, data=json.dumps(subscription), headers=headers) 
  if r.status_code == 201:
    print(r.headers)
  else:
    print(r.status_code)
    print(r.text) 
    sys.exit(1)

  print('Start Context Consumer')
  address = ('0.0.0.0', 8080)
  with HTTPServer(address, ContextConsumerHandler) as server:
    server.serve_forever()