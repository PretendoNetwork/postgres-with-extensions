FROM postgres:16 AS build

RUN apt-get update && \
    apt-get install -y build-essential postgresql-server-dev-16 git && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /build

COPY ./postgres-phash-hamming ./postgres-phash-hamming

RUN cd postgres-phash-hamming && \
    make && \
    make install

FROM postgres:16

# Copy only the compiled extension files from build stage
COPY --from=build /usr/lib/postgresql/16/lib/ /usr/lib/postgresql/16/lib/
COPY --from=build /usr/share/postgresql/16/extension/ /usr/share/postgresql/16/extension/