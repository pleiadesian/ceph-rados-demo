# Ceph-rados-demo

A demo for using libRADOS.

## Prerequisite

Install `librados` and `libradospp`

## Run it

``` bash
mkdir -p build
cd build
cmake ..
make
./demo -c $PATH_TO_CEPH_CFG
```
