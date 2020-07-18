#!/bin/bash
: ${IOTA_IP:?Not found}
: ${DEVICE_ID:=sensor006}
curl -iX DELETE -s "http://$IOTA_IP:4041/iot/devices/$DEVICE_ID" -H 'fiware-service: openiot' -H 'fiware-servicepath: /'
