#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/fs.h>  
#include <linux/cdev.h>  
#include <linux/platform_device.h>  
#include "ymj_bus.h"  

#define CHRDEV_NAME "ymjtest_cdev"

// driver interface  
static struct class *chrdev_class = NULL;  
static struct device *chrdev_device = NULL;  
static dev_t chrdev_devno;  
static struct cdev chrdev_cdev;  


static int chrdev_open(struct inode *inode, struct file *file) {  
	printk(KERN_ALERT "chrdev open!\n");  
	return 0;  
}  

static int chrdev_release(struct inode *inode, struct file *file) {  
	printk(KERN_ALERT "chrdev release!\n");  
	return 0;  
}  

static int chrdev_ioctl(struct inode *inode, struct file *file,  
	unsigned int cmd, unsigned long arg) {  
	printk(KERN_ALERT "chrdev release!\n");  
	return 0;  
}  

// Kernel interface  
static struct file_operations chrdev_fops = {  
	.owner      =   THIS_MODULE,  
	.unlocked_ioctl      =   chrdev_ioctl,  
	.open       =   chrdev_open,  
	.release    =   chrdev_release,  
};  
///////////////////////////////////////////////////
static int y_probe(struct ymj_device *ydev, const struct ymj_driver *ydrv)
{
	printk("probe OK !\n");	
	int ret = 0, err = 0;  
	printk(KERN_ALERT "chrdev probe!\n");  

	// alloc character device number  
	ret = alloc_chrdev_region(&chrdev_devno, 0, 1, CHRDEV_NAME);  
	if (ret) {  
		printk(KERN_ALERT " alloc_chrdev_region failed!\n");  
		goto PROBE_ERR;  
	}  
	printk(KERN_ALERT " major:%d minor:%d\n", MAJOR(chrdev_devno), MINOR(chrdev_devno));  

	cdev_init(&chrdev_cdev, &chrdev_fops);  
	chrdev_cdev.owner = THIS_MODULE;  
	// add a character device  
	err = cdev_add(&chrdev_cdev, chrdev_devno, 1);  
	if (err) {  
		printk(KERN_ALERT " cdev_add failed!\n");  
		goto PROBE_ERR;  
	}  

	// create the device class  
	chrdev_class = class_create(THIS_MODULE, CHRDEV_NAME"_class");  
	if (IS_ERR(chrdev_class)) {  
		printk(KERN_ALERT " class_create failed!\n");  
		goto PROBE_ERR;  
	}  

	// create the device node in /dev  
	chrdev_device = device_create(chrdev_class, NULL, chrdev_devno,  
			NULL, CHRDEV_NAME);  
	if (NULL == chrdev_device) {  
		printk(KERN_ALERT " device_create failed!\n");  
		goto PROBE_ERR;  
	}  

	printk(KERN_ALERT " chrdev probe ok!\n");  
	return 0;  

PROBE_ERR:  
	if (err)  
		cdev_del(&chrdev_cdev);  
	if (ret)   
		unregister_chrdev_region(chrdev_devno, 1);  
	return -1;  
}
/////////////////////////////
static int y_remove(struct ymj_device *ydev)
{
	printk("Remove\n");	
	cdev_del(&chrdev_cdev);  
	unregister_chrdev_region(chrdev_devno, 1);  

	device_destroy(chrdev_class, chrdev_devno);  
	class_destroy(chrdev_class);  
	return 0;	
}

static struct ymj_driver my_driver = {  
	.driver = {
		.name = "ymjtest",
	.probe = y_probe,
	.remove = y_remove,
	},
	.version = "V2.0.0.0",
	.module = THIS_MODULE,
};  

static __init int ymjdrv_init(void) {  
	int ret = 0;  

	ret = ymj_driver_register(&my_driver);  
	printk(KERN_ALERT " ymjdrv_init ok!\n");  
	return ret;  
}  

static __exit void ymjdrv_exit(void) {  
	printk(KERN_ALERT "ymjdrv exit!\n");  
	ymj_driver_unregister(&my_driver);  
}  

module_init(ymjdrv_init);  
module_exit(ymjdrv_exit); 
MODULE_LICENSE("Dual BSD/GPL");  
