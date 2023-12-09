FROM ubuntu:20.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt update

RUN apt install -y clang
RUN apt install -y cmake

COPY ../src /home/src
COPY ../include /home/include
COPY ../leveldb /home/leveldb
ADD ./docmain.cpp /home/main.cpp
ADD ../CMakeLists.txt /home/CMakeLists.txt
COPY start.sh /home/start.sh

CMD ["chmod a+x start.sh"]
CMD ["ls -lt"]
CMD ["./home/start.sh"]

EXPOSE 5060
