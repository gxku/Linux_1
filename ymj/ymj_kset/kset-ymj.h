/*
 *kset-ymj.h
 */
 struct ymj_obj {
	struct kobject kobj;
	int ba1;
	int ba2;
};
#define to_ymj_obj(x)  container_of(x,struct ymj_obj, kobj)

struct ymj_attribute {
	struct attribute attr;
	ssize_t (*show)(struct ymj_obj *ymj, struct ymj_attribute *attr, char *buf);
	ssize_t (*store)(struct ymj_obj *ymj, struct ymj_attribute *attr, const char *buf, size_t count);
};
#define to_ymj_attr(x) container_of(x, struct ymj_attribute, attr)

#define YMJ_ATTR(_name, _mode, _show, _store)   \
struct ymj_attribute ymj_attr_##_name =		\
__ATTR(_name, _mode, _show, _store)

/*#define __ATTR(_name, _mode, _show, _store) {				\
 *	.attr = {.name = __stringify(_name),				\
 *		.mode = VERIFY_OCTAL_PERMISSIONS(_mode) },		\
 *	.show	= _show,						\
 *	.store	= _store,						\
 *}
 */

