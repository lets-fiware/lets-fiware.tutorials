#!/bin/bash
: ${IOTA_IP:?Not found}
: ${DEVICE_ID:=sensor005}
curl -iX POST \
  "http://$IOTA_IP:4041/iot/devices" \
  -H 'Content-Type: application/json' \
  -H 'fiware-service: openiot' \
  -H 'fiware-servicepath: /' \
  -d "{
 \"devices\": [
   {
     \"device_id\":   \"$DEVICE_ID\",
     \"entity_name\": \"urn:ngsi-ld:WeatherObserved:$DEVICE_ID\",
     \"entity_type\": \"Sensor\",
     \"timezone\":    \"Asia/Tokyo\",
     \"protocol\":    \"PDI-IoTA-UltraLight\",
     \"transport\":   \"MQTT\",
     \"attributes\": [
       { \"object_id\": \"d\", \"name\": \"dateObserved\", \"type\": \"DateTime\" },
       { \"object_id\": \"t\", \"name\": \"temperature\", \"type\": \"Number\" },
       { \"object_id\": \"h\", \"name\": \"relativeHumidity\", \"type\": \"Number\" },
       { \"object_id\": \"p\", \"name\": \"atmosphericPressure\", \"type\": \"Number\" }
     ],
     \"static_attributes\": [
       { \"name\":\"location\", \"type\": \"geo:json\", \"value\" : { \"type\": \"Point\", \"coordinates\" : [ 139.7671, 35.68117 ] } }
     ]
   }
 ]
}
"
