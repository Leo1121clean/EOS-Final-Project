obj-m += all_driver.o
KDIR = /lib/modules/5.15.92-v8+/build
CC = /usr/bin/aarch64-linux-gnu-gcc
all:
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- CFLAGS="-std=c99" -C $(KDIR) M=$(shell pwd) modules
	$(CC) -o hw1 hw1.c
clean:
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- CFLAGS="-std=c99" -C $(KDIR) M=$(shell pwd) clean
	$(CC) -o hw1 hw1.c