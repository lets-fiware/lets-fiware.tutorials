#!/bin/bash
: ${IOTA_IP:?Not found}
: ${IOTA_KEY:=4jggokgpepnvsb2uv4s40d59ov}
curl -is -X DELETE "http://$IOTA_IP:4041/iot/services?resource=/iot/d&apikey=$IOTA_KEY" \
-H 'fiware-service: openiot' -H 'fiware-servicepath: /'
