#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import os
import sys
import signal
from http.server import BaseHTTPRequestHandler, HTTPServer
from urllib.parse import parse_qs, urlparse
import json
from bme280 import BME280

try:
  from smbus2 import SMBus
except ImportError:
  from smbus import SMBus

def handler(signum, frame):
  sys.exit(0)

class ContextProviderHandler(BaseHTTPRequestHandler):
  def do_POST(self):
    content_length = int(self.headers['content-length'])
    path = self.path
    if (path != '/v1/weatherObserved/op/query'):
      self.send_response(404)
      return

    body = json.loads(self.rfile.read(content_length).decode('utf-8'))
    e = body['entities'][0]['id']
    if (e != device_id):
      self.send_response(404)
      return
      
    if (debug):
      print('path = {}'.format(path))
      print('body = {}'.format(body))

    response = [
      {
        "id": device_id,
        "type": "WeatherObserved",
        'temperature': {
          'type': 'Number',
          'value': '{:05.2f}'.format(bme280.get_temperature())
        },
        'relativeHumidity': {
          'type': 'Number',
          'value': '{:05.2f}'.format(bme280.get_humidity())
        },
        'atmosphericPressure': {
          'type': 'Number',
          'value': '{:05.2f}'.format(bme280.get_pressure())
        }
      }
    ]

    if (debug):
      print(response)

    self.send_response(200)
    self.send_header('Content-Type', 'application/json; charset=utf-8')
    self.end_headers()
    self.wfile.write(json.dumps(response).encode('utf-8'))
        
if __name__ == '__main__':
  signal.signal(signal.SIGTERM, handler)
  signal.signal(signal.SIGINT, handler)

  bus = SMBus(1)
  bme280 = BME280(i2c_dev=bus)
  device_id = 'urn:ngsi-ld:WeatherObserved:' + os.environ.get('DEVCIE_ID', 'sensor002')
  debug = not not os.environ.get('DEBUG_FLAG', '')

  print('Start Context Provider')
  address = ('0.0.0.0', 8080)
  with HTTPServer(address, ContextProviderHandler) as server:
      server.serve_forever()
