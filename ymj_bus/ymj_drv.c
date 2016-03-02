#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/fs.h>  
#include "ymj_bus.h"  

static int y_probe(struct ymj_device *ydev, const struct ymj_driver *ydrv)
{
	printk("probe OK !\n");	
	return 0;	
}
static int y_remove(struct ymj_device *ydev)
{
	printk("Remove\n");	
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
