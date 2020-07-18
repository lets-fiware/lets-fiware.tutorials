#!/bin/bash
: ${ORION_URL:?Not found}
: ${DEVICE_ID:=lcd002}
curl -iX PATCH "$ORION_URL/v2/entities/urn:ngsi-ld:Lcd:$DEVICE_ID/attrs" \
  -H 'Content-Type: application/json' \
  -H 'fiware-service: openiot' \
  -H 'fiware-servicepath: /' \
  -d '{
  "on": {
      "type" : "command",
      "value" : ""
  }
}'
