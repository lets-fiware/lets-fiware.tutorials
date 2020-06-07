#!/bin/bash
: ${ORION_URL:?Not found}
: ${DEVICE_ID:=sensor006}
curl -siX DELETE \
  "$ORION_URL/v2/entities/urn:ngsi-ld:WeatherObserved:$DEVICE_ID" \
  -H 'fiware-service: openiot' \
  -H 'fiware-servicepath: /'
