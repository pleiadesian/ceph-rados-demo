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

## Output

``` bash
$ ./demo -c /home/wzl/ceph/build/ceph.conf
we just set up a rados cluster object
we just parsed our config options
we just connected to the rados cluster
we just created a new pool named hello_world_pool
we just created an ioctx for our pool
we just wrote new object HelloWorld, with contents
HelloWorld
we read our object HelloWorld, and got back 11 bytes with contents
HelloWorld
```
