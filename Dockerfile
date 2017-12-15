FROM alpine:latest as builder
ENV DENN_RELEASE_VERSION=1.2.0
RUN apk update \
    && apk upgrade \
    && apk add alpine-sdk wget zlib-dev \
    && wget -O /tmp/DENN.tar.gz "https://github.com/Gabriele91/DENN-LITE/archive/r${DENN_RELEASE_VERSION}.tar.gz" \
    && mkdir /tmp/DENN \
    && cd /tmp \
    && tar -xvf DENN.tar.gz -C DENN --strip-components=1 \
    && cd DENN \
    && make release

FROM alpine:latest 
RUN apk update \
    && apk add libgcc libstdc++ zlib \
    && apk upgrade
COPY --from=builder /tmp/DENN/Release/DENN-float /usr/local/bin

#CMD ["./app"]