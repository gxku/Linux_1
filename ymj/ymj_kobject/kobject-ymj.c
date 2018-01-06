#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>

static int ba1 = 0, ba2 = 0, foo = 0; 
static struct kobject *ymj_kobj;

//foo attribute
static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr,
				char *buf)
{
	return sprintf(buf, "%d\n", foo);
}
static ssize_t foo_store(struct kobject *obj, struct kobj_attribute *attr,
				const char *buf, size_t count)
{
	int var = 0,ret = 0;
	ret = kstrtoint(buf, 10, &var);
	if (ret < 0)
		return ret;
	
	foo = var;
	
	return count;
}
static struct kobj_attribute foo_attribute = 
	__ATTR(foo,0644,foo_show,foo_store);


//ba1 ba2 attribute
static ssize_t b_show(struct kobject *kobj, struct kobj_attribute *attr,
				char *buf)
{
	int var = 0;
	if (strcmp(attr->attr.name,"ba1")==0)
		var = ba1;
	else	
		var = ba2;
	return sprintf(buf, "%d\n", var);
}
static ssize_t b_store(struct kobject *obj, struct kobj_attribute *attr,
				const char *buf, size_t count)
{
	int var = 0,ret = 0;
	ret = kstrtoint(buf, 10, &var);
	if (ret < 0)
		return ret;
	if (strcmp(attr->attr.name,"ba1")==0)
		var = ba1;
	else	
		var = ba2;
	
	return count;
}
static struct kobj_attribute ba1_attribute = 
	__ATTR(ba1,0644,b_show,b_store);
static struct kobj_attribute ba2_attribure =
	__ATTR(ba2,0644,b_show,b_store);


static struct attribute *attrs[] = {
	&ba1_attribute.attr,
	&ba2_attribure.attr,
	NULL,
};

static struct attribute_group attr_group = {
	.attrs = attrs,
};


static int __init ymj_kobj_init(void)
{
	int ret=0;

	//create kobject
	ymj_kobj = kobject_create_and_add("kobject_ymj",NULL/*kernel_kobj*/);
	if (!ymj_kobj)
		return -ENOMEM;
	//create files for this kobject
	ret = sysfs_create_group(ymj_kobj,&attr_group);
	if (ret)
		kobject_put(ymj_kobj);

	//create a file for this kobject
	ret = sysfs_create_file(ymj_kobj,&foo_attribute.attr);
	if (ret)
		kobject_put(ymj_kobj);
	return 0;
}

static void __exit ymj_kobj_exit(void)
{
	kobject_put(ymj_kobj);
}

module_init(ymj_kobj_init);
module_exit(ymj_kobj_exit);
MODULE_LICENSE("GPL v2");

