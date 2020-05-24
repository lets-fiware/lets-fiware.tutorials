#!/bin/bash
: ${1:?run.sh <orion_url>}
echo $1
docker run --rm -d --name context_producer01 -e ORION_URL=$1 -e DEBUG_FLAG=true -e DEBUG_FLAG=true --device /dev/i2c-1 context_producer01

