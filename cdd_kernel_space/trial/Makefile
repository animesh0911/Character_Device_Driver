KBUILD_EXTRA_SYMBOLS := /root/Character_Device_Driver/cdd_kernel_space/fifo/Module.symvers

obj-m := call_exported_symbol.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD = $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
install:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) module_install
clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean

