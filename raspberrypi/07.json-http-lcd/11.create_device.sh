#!/bin/bash
: ${IOTA_IP:?Not found}
: ${DEVICE_ID:=lcd001}
: ${DEVICE_URL:?Not found}
curl -iX POST \
  "http://$IOTA_IP:4041/iot/devices" \
  -H 'Content-Type: application/json' \
  -H 'fiware-service: openiot' \
  -H 'fiware-servicepath: /' \
  -d "{
 \"devices\": [
    {
     \"device_id\":   \"$DEVICE_ID\",
     \"entity_name\": \"urn:ngsi-ld:Lcd:$DEVICE_ID\",
     \"entity_type\": \"Lcd\",
     \"timezone\":    \"Asia/Tokyo\",
     \"transport\": \"HTTP\",
     \"endpoint\": \"$DEVICE_URL/iot/$DEVICE_ID\",
     \"commands\": [
        {\"name\": \"on\",\"type\": \"command\"},
        {\"name\": \"off\",\"type\": \"command\"}
       ]
    }
  ]
}
"
