#!/bin/bash
: ${MQTT_IP:?Not found}
docker run --rm -d --name json-mqtt-lcd-01 -e MQTT_IP=$MQTT_IP -e DEBUG_FLAG=true --device /dev/spidev0.0 --device /dev/spidev0.1 --device /dev/mem --privileged fisuda/json-mqtt-lcd-01:1.0
