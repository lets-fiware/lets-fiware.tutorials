FROM debian:buster

RUN apt update && \
    apt -y install python3 python3-pip zlib1g-dev libjpeg-dev && \
    pip3 install numpy RPI.GPIO ST7735 pillow paho-mqtt requests fonts font-roboto && \
    apt remove -y python3-pip && \
    apt clean && \
    apt -y autoremove && \
    rm -rf /var/lib/apt/lists/*

COPY context-consumer.py /

CMD ["/usr/bin/python3", "-u", "/context-consumer.py"]
