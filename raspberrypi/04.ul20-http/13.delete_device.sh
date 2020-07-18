#!/bin/bash
: ${IOTA_IP:?Not found}
: ${DEVICE_ID:=sensor004}
curl -iX DELETE -s "http://$IOTA_IP:4041/iot/devices/$DEVICE_ID" -H 'fiware-service: openiot' -H 'fiware-servicepath: /'
