#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import os
import signal
import datetime
import time
import requests
import json
from bme280 import BME280

try:
  from smbus2 import SMBus
except ImportError:
  from smbus import SMBus

EXIT = False

def handler(signum, frame):
  global EXIT
  EXIT = True

def main():
  signal.signal(signal.SIGTERM, handler)
  signal.signal(signal.SIGINT, handler)

  bus = SMBus(1)
  bme280 = BME280(i2c_dev=bus)

  orion_url = os.environ.get('ORION_URL', 'http://orion:1026').rstrip('/')
  fiware_service = os.environ.get('FIWARE_SERVICE', '')
  fiware_servicepath = os.environ.get('FWARE_SERVICEPATH', '/device')
  id = os.environ.get('DEVICE_ID', 'urn:ngsi-ld:WeatherObserved:sensor001')
  debug = not not os.environ.get('DEBUG_FLAG', '')

  headers = {
    'Fiware-Service': fiware_service,
    'Fiware-ServicePath': fiware_servicepath,
  }

  # Get Entity
  url = orion_url + '/v2/entities/' + id
  r = requests.get(url, headers=headers)
  if (r.status_code == 200):
    # Delete Entity
    r = requests.delete(url, headers=headers)
  
  # Create Entity
  headers['Content-Type'] = 'application/json; charset=utf-8'
  entity = {
    'type': 'WeatherObserved',
    'id': id,
    'dateObserved': {
      'type': 'DateTime',
      'value': '{}'.format(datetime.datetime.utcnow().isoformat(timespec='seconds'))
    },
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
    },
    'location': {
      'type': 'geo:json',
      'value': {
        'type': 'Point',
        'coordinates': [
          139.7671,
          35.68117
        ]
      }
    }
  }
  url = orion_url + '/v2/entities'
  r = requests.post(url, data=json.dumps(entity), headers=headers)
  if r.status_code != 201:
    print(r.status_code)
    print(r.text)  

  url = orion_url + '/v2/entities/' + id + '/attrs'

  while True:
    update = {
      'dateObserved': {
        'type': 'DateTime',
        'value': '{}'.format(datetime.datetime.utcnow().isoformat(timespec='seconds'))
      },
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

    if (debug):
      print(update)

    # Patch Entity
    r = requests.patch(url, data=json.dumps(update), headers=headers)
    if (r.status_code != 204):
      print(r.status_code)

    time.sleep(1)

    if (EXIT):
      break

if __name__ == '__main__':
  main()
