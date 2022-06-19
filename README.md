# gjfish

A tool to count k-mer in graph genome.

## Install

### Requirements
* CMake 3.10

* C++/17

### Instructions

```console
$ git clone https://github.com/xing641/gjfish.git
$ git checkout -b GJfish-0.01 origin/GJfish-0.01
$ cd gjfish && mkdir build && cd build
$ cmake .. && make -j
```
## Using
```shell
$ src_stable ${GFA_DATA_PATH} ${THREAD_NUM} ${HASH_FUNCTION} ${MEMORY_SIZE} ${KMER_NUM}
```
