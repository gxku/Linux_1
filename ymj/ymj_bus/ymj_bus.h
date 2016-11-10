/*
 * Definitions for the virtual LDD bus.
 *
 * $Id: ymjbus.h,v 1.4 2004/08/20 18:49:44 corbet Exp $
 */

//extern struct device ymj_bus;
extern struct bus_type ymj_bus_type;

/*
 * A device type for things "plugged" into the LDD bus.
 */

struct ymj_device {
	char *name;
	struct ymj_driver *driver;
	struct device dev;
};

#define to_ymj_device(dev) container_of(dev, struct ymj_device, dev);

/*
 * The LDD driver type.
 */

struct ymj_driver {
	char *version;
	int (*probe)(struct ymj_device *, const struct ymj_driver *);
	int (*remove)(struct ymj_device *);
	struct module *module;
	struct device_driver driver;
	struct driver_attribute version_attr;
};

#define to_ymj_driver(drv) container_of(drv, struct ymj_driver, driver);


extern int ymj_device_register(struct ymj_device *);
extern void ymj_device_unregister(struct ymj_device *);
extern int ymj_driver_register(struct ymj_driver *);
extern void ymj_driver_unregister(struct ymj_driver *);
