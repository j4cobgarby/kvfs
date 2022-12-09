#!/usr/bin/bash

make
sudo insmod build/kvfsmod.ko
mkdir -p mnt
sudo mount -t kvfs none mnt