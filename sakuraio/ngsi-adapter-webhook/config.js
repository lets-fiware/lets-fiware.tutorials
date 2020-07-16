/*
 * NGSI Adapter for sakura.io (Webhook)
 * https://github.com/lets-fiware/lets-fiware.tutorials
 *
 * Copyright (c) 2020 Kazuhito Suda
 * Licensed under the MIT license.
 */

var config = {}

config.orionUrl = (process.env.ORION_URL || 'http://orion:1026/');
config.fiwareService = (process.env.FIWARE_SERVICE || 'openiot');
config.fiwareServicepath = (process.env.FIWARE_SERVICEPATH || '/sakuraio');
config.entityType = (process.env.ENTITY_TYPE || 'Device');

config.endpoint = (process.env.NGSI_ADAPTER_ENDPOINT || '/v1');
config.port = (process.env.SERVER_PORT || 8080);

config.secret = (process.env.SAKURA_IO_SECRET || 'lets-fiware');

config.debug = (process.env.FIWARE_DEBUG ? (process.env.FIWARE_DEBUG.toLowerCase() == 'true') : false);

module.exports = config;
