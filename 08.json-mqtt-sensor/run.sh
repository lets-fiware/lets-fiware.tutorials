#!/bin/bash
: ${MQTT_IP:?Not found}
docker run --rm -d --name json-mqtt-sensor-01 -e MQTT_IP=$MQTT_IP -e DEBUG_FLAG=true --device /dev/i2c-1 fisuda/json-mqtt-sensor-01:1.0

