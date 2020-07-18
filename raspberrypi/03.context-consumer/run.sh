#!/bin/bash
: ${ORION_URL:?Not found}
: ${CONTEXT_CONSUMER_URL:?Not found}
docker run --rm -d --name context-consumer-01 -p 8080:8080 -e ORION_URL=$ORION_URL -e CONTEXT_CONSUMER_URL=$CONTEXT_CONSUMER_URL -e DEBUG_FLAG=true --device /dev/spidev0.0 --device /dev/spidev0.1 --device /dev/mem --privileged fisuda/context-consumer-01:1.0

