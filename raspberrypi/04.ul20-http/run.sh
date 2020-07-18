#!/bin/bash
: ${IOTA_IP:?Not found}
docker run --rm -d --name ul20-http-01 -e IOTA_IP=$IOTA_IP -e DEBUG_FLAG=true --device /dev/i2c-1 fisuda/ul20-http-01:1.0

