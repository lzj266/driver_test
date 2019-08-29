KERNELDIR ?= /home/linux/ti-processor-sdk-linux-am335x-evm-04.00.00.04/board-support/linux-4.9.28/
PWD := $(shell pwd)

all:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KERNELDIR) M=$(PWD) modules
	arm-linux-gnueabihf-gcc test.c -o app
install:
	sudo cp *.ko  app /tftpboot
clean:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KERNELDIR) M=$(PWD) clean
	rm app

obj-m += demo.o
