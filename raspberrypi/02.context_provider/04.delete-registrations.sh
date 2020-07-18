#!/bin/bash
: ${ORION_URL:?Not found}
: ${REG_ID:?Not found}
curl -isS -H "Fiware-ServicePath: /device" -X DELETE "$ORION_URL/v2/registrations/$REG_ID"
