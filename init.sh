#!/usr/bin/bash

make
insmod build/kvfsmod.ko
mkdir -p /dev/kvs1
mount -t kvfs none /dev/kvs1
