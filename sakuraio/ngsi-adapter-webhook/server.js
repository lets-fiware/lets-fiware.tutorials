/*
 * NGSI Adapter for sakura.io (Webhook)
 * https://github.com/lets-fiware/lets-fiware.tutorials
 *
 * Copyright (c) 2020 Kazuhito Suda
 * Licensed under the MIT license.
 */

'use strict';

const config = require('./config');
const ngsiadapter = require('./routes/ngsi-adapter');
const express = require('express');
const app = express();

const endpoint = config.endpoint;
const port = config.port;

process.on('SIGTERM', () => {
  console.log('Got SIGTERM');
  process.exit(0);
});

app.use(express.json())
app.use(express.urlencoded({ extended: true }));

app.use(endpoint, ngsiadapter);

app.listen(port);

console.log(`NGSI Adapter for sakura.io (Webhook) - ${endpoint}:${port}`);
console.log(`FWIARE-Service: ${config.fiwareService}, FIWARE-ServicePath: ${config.fiwareServicepath}, debug: ${config.debug}`);
