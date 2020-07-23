FROM debian:buster

RUN apt update && \
    apt -y install python3 python3-pip python3-smbus && \
    pip3 install requests pimoroni-bme280 sakuraio && \
    apt remove -y python3-pip && \
    apt clean && \
    apt -y autoremove && \
    rm -rf /var/lib/apt/lists/*

COPY sakuraio-i2c.py /

CMD ["/usr/bin/python3", "-u", "/sakuraio-i2c.py"]
