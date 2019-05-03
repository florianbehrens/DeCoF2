FROM ubuntu:19.04

RUN apt-get update -y
RUN apt-get install -y g++ git libboost-system-dev libboost-test-dev libboost-thread-dev cmake python-pip
RUN pip install -U cpp-coveralls
