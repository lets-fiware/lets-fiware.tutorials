#!/bin/bash
: ${ORION_URL:?Not found}
curl -sS -H "Fiware-ServicePath: /device" "$ORION_URL/v2/registrations" | jq .
