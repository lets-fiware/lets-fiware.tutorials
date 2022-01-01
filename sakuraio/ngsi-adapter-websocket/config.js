/*
 * NGSI Adapter for sakura.io (WebSocket)
 * https://github.com/lets-fiware/lets-fiware.tutorials
 *
 * Copyright (c) 2020-2022 Kazuhito Suda
 * Licensed under the MIT license.
 */
var config = {}

config.orionUrl = (process.env.ORION_URL || 'http://orion:1026/');
config.fiwareService = (process.env.FIWARE_SERVICE || 'openiot');
config.fiwareServicepath = (process.env.FIWARE_SERVICEPATH || '/sakuraio');
config.entityType = (process.env.ENTITY_TYPE || 'Device');

config.debug = (process.env.FIWARE_DEBUG ? (process.env.FIWARE_DEBUG.toLowerCase() == 'true') : false);

config.websocket = (process.env.SAKURA_IO_WEBSOCKET || 'ws://localhost:8080/websocket');

module.exports = config;
