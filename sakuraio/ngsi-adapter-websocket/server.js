/*
 * NGSI Adapter for sakura.io (WebSocket)
 * https://github.com/lets-fiware/lets-fiware.tutorials
 *
 * Copyright (c) 2020-2022 Kazuhito Suda
 * Licensed under the MIT license.
 */

'use strict';

const config = require('./config');
const axios = require('axios');
const WebSocket = require('ws');

const debug = config.debug;
const secret = config.secret;
const orionUrl = config.orionUrl;
const entityType = config.entityType;
const websocketUrl = config.websocket;

process.on('SIGTERM', () => {
  console.log('Got SIGTERM');
  process.exit(0);
});

console.log('NGSI Adapter for sakura.io (WebSocket)');
console.log(`WebSocket: ${websocketUrl}`);
console.log(`Orion URL: ${orionUrl}, EntityType: ${entityType}`);
console.log(`FWIARE-Service: ${config.fiwareService}, FIWARE-ServicePath: ${config.fiwareServicepath}, debug: ${config.debug}`);

const ws = new WebSocket(websocketUrl);

ws.on('open', function () {
    console.log('Connected');
});

ws.on('close', function () {
    console.log('Disconected');
});

ws.on('message', function (data) {
    try {
        if (debug) {
            console.log(data);
        }

        data = JSON.parse(data);

        const payload = data.payload;

        let entity = {};

        switch (data.type) {
            case 'channels':
                for (let i = 0; i < payload.channels.length; i++) {
                    const channel = payload.channels[i];
                    entity[mapper[i].name] = mapper[i].func(channel);
                }
                break;

            case 'location':
                if (payload.coordinate != null) {
                    entity['location'] = attrLocation(payload.coordinate);
                }
                break;

            case 'connection':
                console.log(`connection: ${data.datetime} : is_online : ${payload.is_online}`);
                break;

            case 'keepalive':
                if (debug) {
                    console.log(`keepalive: ${data.datetime}`);
                }
                break;
        }

        if (!!Object.keys(entity).length) {
            entity['id'] = `urn:ngsi-ld:${entityType}:${data.module}`;
            entity['type'] = entityType;
            entity['modifiedAt'] = { type: "DateTime", value: new Date(data.datetime) };

            opUpdate(entity);
            console.log(`${data.module}`);
        }

    } catch (error) {
        console.log(`Exception: ${error.message}`);
    }
    
});

const attrNumber = function attrNumber(channel) {
    return {
        type: 'Number',
        value: channel.value,
        metadata: {
            TimeInstant: {
                type: "DateTime",
                value: new Date(channel.datetime)
            }
        }
    };
}

const attrLocation = function attrLocation(coord) {
    return {
        type: 'geo:json',
        value: {
            type: 'Point',
            coordinates: [ coord.longitude, coord.latitude ]
        },
        metadata: {
            range: {
                type: "Number",
                value: coord.range_m
            }
        }
    };
}

const mapper = [
    { name: 'temperature', func: attrNumber },
    { name: 'relativeHumidity', func: attrNumber },
    { name: 'atmosphericPressure', func: attrNumber },
];

const headers = {
    'FIWARE-Service': config.fiwareService.trim().toLowerCase(),
    'FIWARE-ServicePath': config.fiwareServicepath.trim()
};

function opUpdate(entity) {
    const url = new URL(`/v2/op/update`, orionUrl);

    if (debug) {
        console.log(JSON.stringify(entity));
    }

    axios({
        method: 'post',
        url: url.href,
        headers: headers,
        data: { actionType: 'append', entities: [entity] }
    })
    .then(response => {
        if (response.status != 204) {
            console.log(`opUpdate : ${error.response.status}`);
        }
    })
    .catch(error => {
        console.log(`opUpdate : ${error.response.status}`);
    })
}
