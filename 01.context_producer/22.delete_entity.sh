#!/bin/bash
: ${ORION_URL:?Not found}
: ${DEVICE_ID:=sensor001}
curl -siX DELETE \
  "$ORION_URL/v2/entities/urn:ngsi-ld:WeatherObserved:$DEVICE_ID" \
  -H 'fiware-servicepath: /device'
