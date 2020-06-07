#!/bin/bash
: ${ORION_URL:?Not found}
: ${SUB_ID:?Not found}
curl -isS -H "Fiware-ServicePath: /device" -X DELETE "$ORION_URL/v2/subscriptions/$SUB_ID"
