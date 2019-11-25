PWD := $(shell pwd)
obj-m += icapbit_test.o

all:
	make -C $(KERNEL) M=$(PWD) modules
clean:
	make -C $(KERNEL) M=$(PWD) clean
