#!/bin/bash
: ${ORION_URL:?Not found}
: ${CONTEXT_PROVIDER_URL:?Not found}
curl -iX POST \
  "$ORION_URL/v2/registrations" \
  -H "Content-Type: application/json" \
  -H "Fiware-ServicePath: /device" \
  -d "{
  \"description\": \"Weather Context Source\",
  \"dataProvided\": {
    \"entities\": [
      {
        \"id\": \"urn:ngsi-ld:WeatherObserved:sensor002\",
        \"type\": \"WeatherObserved\"
      }
    ],
    \"attrs\": [
      \"temperature\",
      \"relativeHumidity\",
      \"atmosphericPressure\"
    ]
  },
  \"provider\": {
    \"http\": {
      \"url\": \"$CONTEXT_PROVIDER_URL/v1/weatherObserved\"
    }
  }
}"
