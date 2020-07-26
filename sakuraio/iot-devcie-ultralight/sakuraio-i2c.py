#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import os
import sys
import signal
import math
import time
import sakuraio
from sakuraio.hardware.rpi import SakuraIOSMBus
from bme280 import BME280

try:
    from smbus2 import SMBus
except ImportError:
    from smbus import SMBus

wait = int(os.environ.get('INTERVAL', '5'));
key = os.environ.get('IOTA_KEY', '1234');

def handler(signum, frame):
    sys.exit(0)

def main():
    signal.signal(signal.SIGTERM, handler)
    signal.signal(signal.SIGINT, handler)

    bus = SMBus(1)
    bme280 = BME280(i2c_dev=bus)

    sakuraio = SakuraIOSMBus()

    try:
        while (sakuraio.get_connection_status() and 0x80) == 0 :
            time.sleep(1)

        print("Connected")
  
        while True:
            temperature = bme280.get_temperature()
            pressure = bme280.get_pressure()
            humidity = bme280.get_humidity()
            measures = 'A{}?t|{:05.2f}|h|{:05.2f}|p|{:05.2f}A'.format(key, temperature, humidity, pressure)
            print(measures)
            for i in range(math.ceil(len(measures) / 8)):
                sakuraio.enqueue_tx(i, measures[i * 8:i * 8 + 8])
            sakuraio.send()
            print("Send Data")
            time.sleep(wait)
  
    except Exception as e:
        print(e)

if __name__ == '__main__':
    main()
