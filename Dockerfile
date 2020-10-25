FROM gcc:10.2.0 AS build

ADD . /pkg/

WORKDIR /pkg/

RUN make -e TARGET_DIR=/pkg/bin -e PROGRAM=disease-aggregator

FROM gcc:10.2.0

COPY --from=build /pkg/bin/disease-aggregator /bin/disease-aggregator

ENTRYPOINT ["disease-aggregator"]
