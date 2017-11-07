FROM ubuntu:latest as builder
WORKDIR /root
RUN apt-get update && \
    apt-get -y upgrade && \
    apt-get -y install wget build-essential zlib1g-dev
RUN wget -O DENN.tar.gz https://github.com/Gabriele91/DENN-LITE/archive/r1.1.0.tar.gz && \
    mkdir DENN && \
    tar -xvf DENN.tar.gz -C DENN --strip-components=1 && \
    cd DENN && \
    make release

FROM ubuntu:latest 
RUN apt-get update && \
    apt-get -y upgrade
COPY --from=builder /root/DENN/Release/DENN-float /usr/local/bin

#CMD ["./app"]