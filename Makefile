BUILD_DIR = $(PWD)/build

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(BUILD_DIR) src=$(PWD)/kvfs modules

$(BUILD_DIR):
	mkdir -p "$@"

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
