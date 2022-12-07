obj-m += kvfs.o
kvfs-objs := main.o
BUILD_DIR = $(PWD)/build

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(BUILD_DIR) src=$(PWD) modules

$(BUILD_DIR):
	mkdir -p "$@"

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	rm -rf $(BUILD_DIR)