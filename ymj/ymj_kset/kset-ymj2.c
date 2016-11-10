#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>
#include "kset-ymj.h"

extern void destroy_ymj_obj(struct ymj_obj *ymj);
extern struct ymj_obj *create_ymj_obj(const char *name);
extern int ymj_create_file(struct ymj_obj *y,const struct ymj_attribute *attr);
static struct ymj_obj *ymjobj;

static int var = 100;

static ssize_t y_show(struct ymj_obj *ymjobj, struct ymj_attribute *attr,
				char *buf)
{
	return sprintf(buf, "yyy:%d\n", var);
}
static ssize_t y_store(struct ymj_obj *ymjobj, struct ymj_attribute *attr,
				const char *buf, size_t count)
{
	int ret;
	ret = kstrtoint(buf,10,&var);
	if (ret <0)
		return ret;
	
	return count;
}

YMJ_ATTR(yyy, 0664, y_show, y_store);
////////////////////////////////////
static int __init ymj_kset2_init(void)
{
	ymjobj = create_ymj_obj("test");
	if (!ymjobj)
		return -EINVAL;
	
	ymj_create_file(ymjobj,&ymj_attr_yyy);

	return 0;	
}

static void __exit ymj_kset2_exit(void)
{
	destroy_ymj_obj(ymjobj);	
}
module_init(ymj_kset2_init);
module_exit(ymj_kset2_exit);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("ymj");
