#!/bin/bash
: ${IOTA_IP:?Not found}
docker run --rm -d --name json-http-sensor-01 -e IOTA_IP=$IOTA_IP -e DEBUG_FLAG=true --device /dev/i2c-1 fisuda/json-http-sensor-01:1.0
