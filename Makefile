# See: https://stackoverflow.com/questions/15910064/how-to-compile-a-linux-kernel-module-using-std-gnu99

TARGET_MODULE := one

# Permit a user-defined kernel version with the KVER variable (#6)
KVER ?= $(shell uname -r)
# -- like /lib/modules/5.15.0-57-generic/build/certs/signing_key.x509
BUILDSYSTEM_DIR ?= /lib/modules/$(KVER)/build

PWD := $(shell pwd)
obj-m := $(TARGET_MODULE).o
ccflags-y := -std=gnu99 -Wno-declaration-after-statement

define title
	@echo -ne '\n\033[32m'
	@echo -n $1
	@echo -e '\033[0m'
endef

all: \
	check \
	build \
	sign \
	install \
	load \
	create \
	test


clean: \
	unbuild \
	delete


key:
	@$(call title, "Creating keys")
	openssl req -new -x509 -newkey rsa:2048 -days 36500 -keyout MOK.priv -outform DER -out MOK.der -nodes -subj "/CN=TinmarinoUnsafe/"
	#
	# TODO backup if there is one key
	cp MOK.der $(BUILDSYSTEM_DIR)/certs/signing_key.x509
	cp MOK.priv $(BUILDSYSTEM_DIR)/certs/signing_key.pem
	#
	@echo "\e[31;1mPlease enter a password you will be asked for on reboot:\e[0m"
	mokutil --import MOK.der
	@echo "\e[31;1mNow you must: 1/ reboot, 2/ Select Unroll MOK, 3/ Enter password you previously gave\e[0m"


check:
	@$(call title, "Checking")
	@if [ ! -f MOK.der ] || [ ! -f MOK.priv ]; then \
		echo -n '\e[31m'; \
		echo 'Error: you must create keys before'; \
		echo 'Tip: Run: make key'; \
		echo 'Tip: Read README.md file'; \
		echo '\e[0m'; \
	  exit 1; \
	fi


build:
	# Run kernel build system to make module
	@$(call title, "Compiling")
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) modules


unbuild:
	# run kernel build system to cleanup in current directory
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) clean
	#rm -f MOK.priv MOK*.der


sign:
	@$(call title, "Signing with the generated self-signed keys")
	cp one.ko one.ko.bck
	/usr/src/linux-headers-$(shell uname -r)/scripts/sign-file sha256 MOK.priv MOK.der one.ko


install:
	@$(call title, "Installing system wide")
	$(MAKE) -C $(BUILDSYSTEM_DIR) M=$(PWD) modules_install

uninstall:
	rm $(BUILDSYSTEM_DIR)/extra/one.ko

load:
	modprobe one

unload:
	modprobe -r one

local_load:
	insmod ./$(TARGET_MODULE).ko

local_unload:
	rmmod ./$(TARGET_MODULE).ko


create:
	@$(call title, "Creating node device /dev/one")
	mknod /dev/one c $(shell cat /proc/devices | grep one$ | cut -d ' ' -f1) 0

delete:
	rm /dev/one


test:
	@$(call title, "Testing")
	@if [ "$(shell xxd -p -l 10 /dev/one)" = "ffffffffffffffffffff" ]; then \
		echo "\e[32mSUCCESS\e[0m"; \
		exit 0; \
	else \
		echo "\e[31mFAILED\e[0m"; \
		exit 1; \
	fi
