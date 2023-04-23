# Run coverage test
```
$ mkdir build_coverage
$ cd build_coverage
$ cmake -DCMAKE_BUILD_TYPE=Coverage ..
$ make coverage
```
The report is in `build_coverage/report/index.html`

# Run integration test script
```
$ ./tests/integration.sh 
```