#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import sys
import signal
import time
import sakuraio
from sakuraio.hardware.rpi import SakuraIOSMBus
from bme280 import BME280

try:
    from smbus2 import SMBus
except ImportError:
    from smbus import SMBus

WAIT = 60

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
            sakuraio.enqueue_tx(0, float('{:05.2f}'.format(bme280.get_temperature())))
            sakuraio.enqueue_tx(1, float('{:05.2f}'.format(bme280.get_humidity())))
            sakuraio.enqueue_tx(2, float('{:05.2f}'.format(bme280.get_pressure())))
            sakuraio.send()
            print("Send Data")
            time.sleep(WAIT)
  
    except Exception as e:
        print(e)

if __name__ == '__main__':
    main()
