obj-m := ymj_bus.o ymj_dev.o ymj_drv.o ymj_drv_cdev.o 
#CROSS_COMPILE=/home/root_work/am335x_4220_tmp/mimi_am335x_4220_v2/cross_compiler/linux-devkit/sysroots/x86_64-arago-linux/usr/bin/arm-linux-gnueabihf-
CROSS_COMPILE=
  
#KERNEL_DIR := /home/root_work/am335x_4220_tmp/mimi_am335x_4220_v2/source/linux-4.1.6+gitAUTOINC+52c4aa7cdb-g52c4aa7/
KERNEL_DIR := /lib/modules/3.13.0-32-generic/build/ 
PWD := $(shell pwd)  
    
default:  
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules  
	#$(MAKE) ARCH=arm CROSS_COMPILE=${CROSS_COMPILE} -C $(KERNEL_DIR) M=$(PWD) modules  
  
clean:  
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean  
	-rm *~


