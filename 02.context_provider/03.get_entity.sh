#!/bin/bash
: ${ORION_URL:?Not found}
curl -sS -H "Fiware-ServicePath: /device" "$ORION_URL/v2/entities/urn:ngsi-ld:WeatherObserved:sensor002" | jq .
