#obj-m += kvfsmod.o
#kvfsmod-objs := main.o kvfs.o keyfile.o mkkey.o delkey.o
BUILD_DIR = $(PWD)/build

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(BUILD_DIR) src=$(PWD)/src modules

$(BUILD_DIR):
	mkdir -p "$@"

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
