FROM ubuntu:20.04

WORKDIR /collector

# Install dependencies
RUN apt-get update && \
    apt-get -y install python3 && \
    apt-get -y install python3-pip && \
    apt-get -y install python3-dev && \
    apt-get -y install build-essential

# Only copy the requirements so it gets cached
COPY ./telemetry-collector/requirements.txt /collector
RUN pip install -r requirements.txt


COPY ./telemetry-collector/tests/test-requirements.txt /collector
RUN pip install -r ./test-requirements.txt

RUN apt-get -y install lcov

# Copy over relevant code
COPY ./telemetry-collector/src /collector/src
COPY ./telemetry-collector/Makefile /collector/Makefile

COPY ./car-bsp /car-bsp

RUN make

ENTRYPOINT ["./collector"]
