# Latest gcc
FROM gcc:latest AS build

# Copy sources
COPY . /src

# Build
WORKDIR /src
RUN make
RUN make install


# Ubuntu 22.04
FROM ubuntu:22.04 AS container

# Copy binary from build
COPY --from=build src/logfilegen /usr/bin

# Entrypoint
ENTRYPOINT [ "/usr/bin/logfilegen" ]
