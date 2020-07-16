FROM node:12.18.2-slim
COPY package.json /
RUN mkdir routes && \
    npm install
COPY server.js /
COPY config.js /
COPY ./routes/ngsi-adapter.js /routes
ENTRYPOINT ["/usr/local/bin/node"]
CMD ["server.js"]
