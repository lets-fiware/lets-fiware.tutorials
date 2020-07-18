#!/bin/bash
: ${IOTA_IP:?Not found}
: ${IOTA_KEY:=8f9z57ahxmtzx21oczr5vaabot}
curl -s -X DELETE "http://$IOTA_IP:4041/iot/services?resource=&apikey=$IOTA_KEY" \
-H 'fiware-service: openiot' -H 'fiware-servicepath: /' | jq .
