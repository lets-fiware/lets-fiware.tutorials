FROM debian:buster

RUN apt update && \
    apt -y install python3 python3-pip zlib1g-dev libjpeg-dev && \
    pip3 install requests pimoroni-bme280 smbus2 paho-mqtt && \
    apt remove -y python3-pip && \
    apt clean && \
    apt -y autoremove && \
    rm -rf /var/lib/apt/lists/*

COPY ul20-mqtt.py /

CMD ["/usr/bin/python3", "-u", "/ul20-mqtt.py"]
