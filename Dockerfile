FROM ubuntu:24.04

RUN apt -y update
RUN apt -y install --fix-missing
RUN apt -y install build-essential cmake
RUN apt -y install libgmp-dev catch2