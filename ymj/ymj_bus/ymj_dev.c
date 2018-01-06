#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/fs.h>  
#include <linux/of.h>  
#include "ymj_bus.h"  


#if 0 //for device tree

static struct ymj_device my_device; 

static __init int ymjdev_init(void) {  
	struct device_node *my_node;
	char  *vstr;
	int ret = 0;  
	//struct device_node *of_find_compatible_node(struct device_node *from,const char *type, const char *compatible);
	my_node=of_find_compatible_node(NULL,NULL,"ymj-test");
	//int of_property_read_string(struct device_node *np, const char *propname, const char **out_string);
	ret=of_property_read_string(my_node,"ymjprop",&vstr);
	if (ret)
		return ret;

	my_device.name = vstr;
	ret = ymj_device_register(&my_device);  
	printk(KERN_ALERT " ymjdev2_init ok!\n"); 
	if (ret)
		return ret;
	
	return ret;  
}  
#else  //original 

static struct ymj_device my_device = {  
	.name   =   "ymjtest",  
};  

static __init int ymjdev_init(void) {  
	int ret = 0;  
	ret = ymj_device_register(&my_device);  
	printk(KERN_ALERT " ymjdev_init ok!\n"); 
	
	return ret;  
}  
#endif


static __exit void ymjdev_exit(void) {  
	printk(KERN_ALERT "ymjdev exit!\n");  
	ymj_device_unregister(&my_device);  
}  

module_init(ymjdev_init);  
module_exit(ymjdev_exit); 
MODULE_LICENSE("Dual BSD/GPL");  
