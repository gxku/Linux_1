#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include "ymj_bus.h"

MODULE_LICENSE("Dual BSD/GPL");
static char *Version = "$Revision: 1.9 $";




//////////////////////////////////////////////////
/*
 * Match LDD devices to drivers.  Just do a simplname test.
 */
static int ymj_match(struct device *dev, struct device_driver *driver)
{
	struct ymj_device *ymj_dev;
	ymj_dev = to_ymj_device(dev);
 	printk("match !\n");

	return !strncmp(ymj_dev->name, driver->name, strlen(driver->name));
}

/*
 * The LDD bus device.
 */
static void ymj_bus_release(struct device *dev)
{
	printk(KERN_DEBUG "ymjbus release\n");
}
	
struct device ymj_bus = {
	.init_name   = "ymj0",
	.release  = ymj_bus_release
};


/*
 * And the bus type.
 */
struct bus_type ymj_bus_type = {
	.name = "ymj",
	.match = ymj_match,
};

/*
 * Export a simple attribute.
 */
static ssize_t show_bus_version(struct bus_type *bus, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s\n", Version);
}

static BUS_ATTR(version, S_IRUGO, show_bus_version, NULL);
////////////////////////////////////////////////////

/*
 * LDD devices.
 */
/*
 * For now, no references to LDDbus devices go out which are not
 * tracked via the module reference count, so we use a no-op
 * release function.
 */
static void ymj_dev_release(struct device *dev)
{ }

int ymj_device_register(struct ymj_device *ymjdev)
{
	ymjdev->dev.bus = &ymj_bus_type;
	ymjdev->dev.parent = &ymj_bus;
	ymjdev->dev.release = ymj_dev_release;
	dev_set_name(&ymjdev->dev, "%s_dev", ymjdev->name);
//	ymjdev->dev.init_name=ymjdev->name;
	return device_register(&ymjdev->dev);
}
EXPORT_SYMBOL(ymj_device_register);

void ymj_device_unregister(struct ymj_device *ymjdev)
{
	device_unregister(&ymjdev->dev);
}
EXPORT_SYMBOL(ymj_device_unregister);

/*
 * Crude driver interface.
 */


static ssize_t show_version(struct device_driver *driver, char *buf)
{
	struct ymj_driver *ldriver = to_ymj_driver(driver);

	sprintf(buf, "%s\n", ldriver->version);
	return strlen(buf);
}
		

int ymj_driver_register(struct ymj_driver *driver)
{
	int ret;
	driver->driver.bus = &ymj_bus_type;
	ret = driver_register(&driver->driver);
	if (ret)
		return ret;
	driver->version_attr.attr.name = "version";
	driver->version_attr.attr.mode = S_IRUGO;
	driver->version_attr.show = show_version;
	driver->version_attr.store = NULL;
	return driver_create_file(&driver->driver, &driver->version_attr);
return 0;
}

void ymj_driver_unregister(struct ymj_driver *driver)
{
	driver_unregister(&driver->driver);
}
EXPORT_SYMBOL(ymj_driver_register);
EXPORT_SYMBOL(ymj_driver_unregister);

///////////////////////////////////////////////////
static int __init ymj_bus_init(void)
{
	int ret;

	ret = bus_register(&ymj_bus_type);
	if (ret)
		return ret;
	if (bus_create_file(&ymj_bus_type, &bus_attr_version))
		printk(KERN_NOTICE "Unable to create version attribute\n");
	ret = device_register(&ymj_bus);
	if (ret)
		printk(KERN_NOTICE "Unable to register ymj0\n");

	return ret;
}

static void ymj_bus_exit(void)
{
	device_unregister(&ymj_bus);
	bus_unregister(&ymj_bus_type);
}

module_init(ymj_bus_init);
module_exit(ymj_bus_exit);
