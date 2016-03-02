#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include "kset-ymj.h"

static struct kset *ymj_kset;

static ssize_t ymj_attr_show(struct kobject *kobj,
			     struct attribute *attr,
			     char *buf)
{
	struct ymj_attribute *attribute;
	struct ymj_obj *ymj;

	attribute = to_ymj_attr(attr);
	ymj = to_ymj_obj(kobj);

	if (!attribute->show)
		return -EIO;

	return attribute->show(ymj, attribute, buf);
}

/*
 * Just like the default show function above, but this one is for when the
 * sysfs "store" is requested (when a value is written to a file.)
 */
static ssize_t ymj_attr_store(struct kobject *kobj,
			      struct attribute *attr,
			      const char *buf, size_t len)
{
	struct ymj_attribute *attribute;
	struct ymj_obj *ymj;

	attribute = to_ymj_attr(attr);
	ymj = to_ymj_obj(kobj);

	if (!attribute->store)
		return -EIO;

	return attribute->store(ymj, attribute, buf, len);
}

/* Our custom sysfs_ops that we will associate with our ktype later on */
static const struct sysfs_ops ymj_sysfs_ops = {
	.show = ymj_attr_show,
	.store = ymj_attr_store,
};

/*
 * The release function for our object.  This is REQUIRED by the kernel to
 * have.  We free the memory held in our object here.
 *
 * NEVER try to get away with just a "blank" release function to try to be
 * smarter than the kernel.  Turns out, no one ever is...
 */
static void ymj_release(struct kobject *kobj)
{
	struct ymj_obj *ymj;

	ymj = to_ymj_obj(kobj);
	kfree(ymj);
}


/*
 * More complex function where we determine which variable is being accessed by
 * looking at the attribute for the "ba1" and "ba2" files.
 */
static ssize_t b_show(struct ymj_obj *ymj_obj, struct ymj_attribute *attr,
		      char *buf)
{
	int var;

	if (strcmp(attr->attr.name, "ba1") == 0)
		var = ymj_obj->ba1;
	else
		var = ymj_obj->ba2;
	return sprintf(buf, "%d\n", var);
}

static ssize_t b_store(struct ymj_obj *ymj_obj, struct ymj_attribute *attr,
		       const char *buf, size_t count)
{
	int var, ret;

	ret = kstrtoint(buf, 10, &var);
	if (ret < 0)
		return ret;

	if (strcmp(attr->attr.name, "ba1") == 0)
		ymj_obj->ba1 = var;
	else
		ymj_obj->ba2 = var;
	return count;
}

static struct ymj_attribute ba1_attribute =
	__ATTR(ba1, 0664, b_show, b_store);
static struct ymj_attribute ba2_attribute =
	__ATTR(ba2, 0664, b_show, b_store);

/*
 * Create a group of attributes so that we can create and destroy them all
 * at once.
 */
static struct attribute *ymj_default_attrs[] = {
	&ba1_attribute.attr,
	&ba2_attribute.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

/*
 * Our own ktype for our kobjects.  Here we specify our sysfs ops, the
 * release function, and the set of default attributes we want created
 * whenever a kobject of this type is registered with the kernel.
 */
static struct kobj_type ymj_ktype = {
	.sysfs_ops = &ymj_sysfs_ops,
	.release = ymj_release,
	.default_attrs = ymj_default_attrs,
};
//////////////////////////////////////////////////////
//create kobject for ymj_kset
static struct ymj_obj* create_ymj_obj(const char *name)
{
	struct ymj_obj *ymjobj;
	int ret;
	ymjobj = kzalloc(sizeof(*ymjobj),GFP_KERNEL);
	if (!ymjobj)
		return NULL;

	ymjobj->kobj.kset = ymj_kset;
	ret = kobject_init_and_add(&ymjobj->kobj,&ymj_ktype, NULL, "%s", name);
	if (ret){
		kobject_put(&ymjobj->kobj);	
		return NULL;
	}

	//send the uevent that the kobject was added to system
	kobject_uevent(&ymjobj->kobj,KOBJ_ADD);

	return ymjobj;
}

//destroy kobject for ymj_kset
static void destroy_ymj_obj(struct ymj_obj *ymjobj)
{
	kobject_put(&ymjobj->kobj);
}

int ymj_create_file(struct ymj_obj *ymjobj,const struct ymj_attribute *attr)
{
     int error;
         error = sysfs_create_file(&ymjobj->kobj, &attr->attr);
     return error;
}

EXPORT_SYMBOL_GPL(create_ymj_obj);
EXPORT_SYMBOL_GPL(destroy_ymj_obj);
EXPORT_SYMBOL_GPL(ymj_create_file);

////////////////////////////////////
static int __init ymj_kset_init(void){
	//create a kset named "kset_ymj" under /sys/kernel/
	ymj_kset = kset_create_and_add("kset_ymj", NULL ,kernel_kobj);
	if (!ymj_kset)
		return -ENOMEM;
	return 0;	
}

static void __exit ymj_kset_exit(void){
	kset_unregister(ymj_kset);
}

module_init(ymj_kset_init);
module_exit(ymj_kset_exit);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("ymj");
