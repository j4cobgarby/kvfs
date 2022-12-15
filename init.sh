#!/usr/bin/bash

cd kvfs
make
cd ..
insmod build/kvfsmod.ko
mkdir -p /dev/kvs1
mount -t kvfs none /dev/kvs1 -o `getent group kvfs | cut -d: -f3`
