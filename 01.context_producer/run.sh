#!/bin/bash
: ${ORION_URL:?Not found}
docker run --rm -d --name context-producer-01 -e ORION_URL=$ORION_URL -e DEBUG_FLAG=true --device /dev/i2c-1 fisuda/context-producer-01:1.0
