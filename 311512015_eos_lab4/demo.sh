#!/bin/sh

set -x
# set -e

rmmod -f mydev
insmod mydev.ko

./writer jacob &
./reader 192.168.100.26 8888 /dev/mydev
