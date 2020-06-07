#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import os
import sys
import signal
from http.server import BaseHTTPRequestHandler, HTTPServer
from urllib.parse import parse_qs, urlparse
import json
import ST7735
from PIL import Image, ImageDraw

def handler(signum, frame):
  sys.exit(0)

class LcdHTTPRequestHandler(BaseHTTPRequestHandler):
  def do_POST(self):
    parsed_path = urlparse(self.path)

    content_length = int(self.headers['content-length'])
    body = '{}'.format(self.rfile.read(content_length).decode('utf-8'))

    cmd = json.loads(body)

    if (debug):
      print('path = {}'.format(self.path))
      print(cmd)

    if cmd.get('on') != None:
      cmd['on'] = 'OK'
      disp.display(fiware_img)

    if cmd.get('off') != None:
      cmd['off'] = 'OK'
      disp.display(blank_img)

    self.send_response(200)
    self.send_header('Content-Type', 'application/json; charset=utf-8')
    self.end_headers()
    self.wfile.write(json.dumps(cmd).encode('utf-8'))
        
if __name__ == '__main__':
  signal.signal(signal.SIGTERM, handler)
  signal.signal(signal.SIGINT, handler)

  device_id = 'urn:ngsi-ld:Lcd:' + os.environ.get('DEVCIE_ID', 'lcd001')
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
  
  # Initialize display.
  disp.begin()
  fiware_img = Image.open('./fiware.png')
  blank_img = Image.new('RGB', (disp.width, disp.height), color=(0, 0, 0))

  print('Start Lcd device server')
  address = ('0.0.0.0', 8080)
  with HTTPServer(address, LcdHTTPRequestHandler) as server:
    server.serve_forever()
