TARGET_MODULE:=one

BUILDSYSTEM_DIR:=/lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd)
obj-m := $(TARGET_MODULE).o
ccflags-y := -std=gnu99

all: 
	# run kernel build system to make module
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) modules

clean:
	# run kernel build system to cleanup in current directory
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) clean

load:
	insmod ./$(TARGET_MODULE).ko

unload:
	rmmod ./$(TARGET_MODULE).ko

create:
	mknod /dev/one c $(shell cat /proc/devices | grep one$ | cut -d ' ' -f1) 0

delete:
	rm /dev/one
