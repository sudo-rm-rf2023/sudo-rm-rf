# Run coverage test
```
$ mkdir build_coverage
$ cd build_coverage
$ cmake -DCMAKE_BUILD_TYPE=Coverage ..
$ make coverage
```
The report is in `build_coverage/report/index.html`

# Run all tests (unittests + integration tests)
```
$ mkdir build
$ cd build
$ cmake ..
$ make && make test
```

# Run integration test script
```
$ cp build/bin/server tests/integration_tests
$ cd tests/integration_tests
$ ./integration.sh -c ./server
```
Or
```
$ cd build/bin
$ ./integration.sh -c ./server
```