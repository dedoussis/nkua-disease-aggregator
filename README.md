# disease-aggregator

C/C++ exercise that implements a distributed data processing tool that generates and serves disease statistics upon request.

The solution (almost) adheres to the [Google C++ style guide](https://google.github.io/styleguide/cppguide.html) and to the [Cannonical Project Structure](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1204r0.html).


The app consists of a parent process that orchestrates multiple worker processes that process and control the input data. The parent process is also responsible for aggregating the data generated by the workers (hence aggregator). The communication between the parent and the workers is achieved through FIFO-like pipes using a simple communication protocol. Messages are sent and received as bitstrings divided into two parts, the header and the payload.


The tool provides a CLI interface and can be used as follows:
```bash
./diseaseAggregator –w $NUM_WORKERS -b $BUFFER_SIZE -i $INPUT_DIR
```
where:
* `$NUM_WORKERS` is the number of worker processes that are going to be spawned by the app
* `$BUFFER_SIZE` is the maximum buffer size that will be used for reading and writing as part of the IPC
* `$INPUT_DIR` is the directory containing all the input data. The [`create_infiles.sh`](#create-test-data) script can be used to generate random input data.

## Build & Run
### On host
Requires C++20
```bash
make
./disease-aggregator -w 3 -b 2 -i input_dir.tmp
```

### On docker
Requires [docker](https://www.docker.com/get-started)
```bash
make build-docker
docker run --rm -it -v "$(shell pwd):/runner/" disease-aggregator -w 3 -b 2 -i /runner/input_dir.tmp
```

## Create test data
```bash
./create_infiles.sh $DISEASES_FILE $COUNTRIES_FILE $INPUT_DIR $NUM_FILES_PER_DIR
```
where:
* `$DISEASES_FILE` is a file containing disease names
* `$COUNTRIES_FILE` is a file containing country names
* `$INPUT_DIR` is the name of the directory in which the test data are to be dumped
* `$NUM_FILES_PER_DIR` is the number of entries that each input file has

## TODOs
* Add unit tests
* Split comminucation to each worker into 2 pipes. One for reading from worker, and one for writing to worker. Employ the use of the select system call to not wait on slow workers.
* Implement all the functions of the aggreagator

## Copyright
The spec of this exercise belongs to the Department Of Informatics & Telecominucations of the NKUA university.
