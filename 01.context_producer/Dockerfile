FROM python:3

COPY requirements.txt /
RUN pip3 install -r requirements.txt
COPY context_producer.py /

CMD ["/usr/local/bin/python3", "-u", "/context_producer.py"]
