#!/bin/bash
docker run --rm -d --name json-http-lcd-01 -p 8080:8080 -e DEBUG_FLAG=true --device /dev/spidev0.0 --device /dev/spidev0.1 --device /dev/mem --privileged fisuda/json-http-lcd-01:1.0
