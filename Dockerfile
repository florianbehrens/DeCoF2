FROM ubuntu:18.04

RUN apt-get update -qqy
RUN apt-get install -qqy g++ clang git libboost-system-dev libboost-test-dev libboost-thread-dev cmake make python-pip
RUN pip install -U cpp-coveralls
