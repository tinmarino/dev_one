TARGET_MODULE:=one

BUILDSYSTEM_DIR:=/lib/modules/$(shell uname -r)/build
PWD:=$(shell pwd)
obj-m := $(TARGET_MODULE).o
# See: https://stackoverflow.com/questions/15910064/how-to-compile-a-linux-kernel-module-using-std-gnu99
ccflags-y := -std=gnu99 -Wno-declaration-after-statement

build: 
	# run kernel build system to make module
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) modules

clean:
	# run kernel build system to cleanup in current directory
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) clean
	rm -f MOK.priv MOK*.der

key:
	echo "Creating key"
	openssl req -new -x509 -newkey rsa:2048 -days 36500 -keyout MOK.priv -outform DER -out MOK.der -nodes -subj "/CN=TinmarinoUnsafe/"
	#
	echo "\e[31;1mPlease enter a password you will be asked for on reboot:\e[0m"
	mokutil --import MOK.der
	echo "\e[31;1mNow you must: 1/ reboot, 2/ Select Unroll MOK, 3/ Enter password you previously gave\e[0m"

sign:
	echo "Signing the driver with the generated keys"
	cp one.ko one.ko.bck
	/usr/src/linux-headers-$(shell uname -r)/scripts/sign-file sha256 MOK.priv MOK.der one.ko

load:
	insmod ./$(TARGET_MODULE).ko

unload:
	rmmod ./$(TARGET_MODULE).ko

create:
	mknod /dev/one c $(shell cat /proc/devices | grep one$ | cut -d ' ' -f1) 0

delete:
	rm /dev/one

test:
	[ "$(shell xxd -p -l 10 /dev/one)" = "ffffffffffffffffffff" ] \
		&& echo "\e[32mSUCCESS\e[0m" \
		|| echo "\e[31mFAILED\e[0m"
