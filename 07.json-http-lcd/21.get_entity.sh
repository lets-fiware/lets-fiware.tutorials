#!/bin/bash
: ${ORION_URL:?Not found}
: ${DEVICE_ID:=lcd001}
curl -sX GET \
  "$ORION_URL/v2/entities/urn:ngsi-ld:Lcd:$DEVICE_ID" \
  -H 'fiware-service: openiot' \
  -H 'fiware-servicepath: /' | jq .
