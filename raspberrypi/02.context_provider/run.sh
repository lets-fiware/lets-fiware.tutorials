#!/bin/bash
docker run --rm -d --name context-provider-01 -e DEBUG_FLAG=true -p 8080:8080 --device /dev/i2c-1 fisuda/context-provider-01:1.0
