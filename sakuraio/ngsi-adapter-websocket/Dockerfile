FROM node:12.18.2-slim
COPY package.json /
RUN mkdir routes && \
    npm install
COPY server.js /
COPY config.js /
ENTRYPOINT ["/usr/local/bin/node"]
CMD ["server.js"]
