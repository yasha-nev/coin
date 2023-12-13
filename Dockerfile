FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt update

RUN apt install -y clang
RUN apt install -y cmake

COPY src /home/src
COPY include /home/include
COPY leveldb /home/leveldb
ADD CMakeLists.txt /home/CMakeLists.txt
ADD mainer.cpp /home/main.cpp
ADD start.sh /home/start.sh

CMD ["./home/start.sh"]

EXPOSE 5060
