/*
 * NGSI Adapter for sakura.io (Webhook)
 * https://github.com/lets-fiware/lets-fiware.tutorials
 *
 * Copyright (c) 2020 Kazuhito Suda
 * Licensed under the MIT license.
 */

'use strict';

const config = require('../config');
const axios = require('axios');
const crypto = require('crypto');
const express = require('express');
const router = express.Router();

const debug = config.debug;
const secret = config.secret;
const orionUrl = config.orionUrl;
const entityType = config.entityType;
const headers = {
    'FIWARE-Service': config.fiwareService.trim().toLowerCase(),
    'FIWARE-ServicePath': config.fiwareServicepath.trim()
};

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

router.post('/', function(req, res, next) {
    let status = 500;
    let module = 'Unknown';

    try {
        if (debug) {
            console.log(JSON.stringify(req.body));
        }

        const sig = req.header('X-Sakura-Signature');

        if (sig == null || sig != getSha1(secret, JSON.stringify(req.body))) {
            throw new Error('Error - X-Sakura-Signature');
        }

        module = req.body.module;
        const payload = req.body.payload;

        let entity = {};

        switch (req.body.type) {
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
                console.log(`connection: ${datetime} : is_online : ${payload.is_online}`);
                break;
        }

        if (!!Object.keys(entity).length) {
            entity['id'] = `urn:ngsi-ld:${entityType}:${req.body.module}`;
            entity['type'] = entityType;
            entity['modifiedAt'] = { type: "DateTime", value: new Date(req.body.datetime) };

            opUpdate(entity);
        }

        status = 200;

    } catch (error) {
        console.log(`Exception: ${error.message}`);
    }

    res.sendStatus(status);
    console.log(`${module} : ${status}`);
});

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

function getSha1(secret, message) {
    return  crypto.createHmac('sha1', secret).update(message).digest('hex');
}

module.exports = router;
