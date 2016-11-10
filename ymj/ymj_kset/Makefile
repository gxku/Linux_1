obj-m := kset-ymj.o kset-ymj2.o  
CROSS_COMPILE=/home/root_work/am335x_4220_tmp/mimi_am335x_4220_v2/cross_compiler/linux-devkit/sysroots/x86_64-arago-linux/usr/bin/arm-linux-gnueabihf-
  
KERNEL_DIR := /home/root_work/am335x_4220_tmp/mimi_am335x_4220_v2/source/linux-4.1.6+gitAUTOINC+52c4aa7cdb-g52c4aa7/
PWD := $(shell pwd)  
    
default:  
	$(MAKE) ARCH=arm CROSS_COMPILE=${CROSS_COMPILE} -C $(KERNEL_DIR) M=$(PWD) modules  
  
clean:  
	-rm *~
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean  


