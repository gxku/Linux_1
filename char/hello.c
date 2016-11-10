#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>

#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include "hello.h"
int HELLO_MAJOR = 0;
int HELLO_MINOR = 0;
#define NUMBER_OF_DEVICES 1

struct class *my_class;
struct cdev cdev;
dev_t devno;





 static int hello_open(struct inode *inode, struct file *file)  
 {  
      printk("#########open######\n");  
      return 0;  
 }  
   
   
 static int hello_close(struct inode *inode, struct file *file)  
 {  
     printk("<1>#########release######\n");  
     return 0;  
 }  
   
   
 static int hello_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)  
 {  
      printk("#########read######\n");  
      return count;  
 }  


 static int hello_write (struct file *filp, const char __user *buf, size_t count,loff_t *f_pos)  
 {  
      printk("#########write######\n");  
 }  

static int hello_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)  
 {
 
    int err = 0;
    int ret = 0;
    int ioarg = 0;


printk("[ymj] arg=%x\n",arg);
printk("[ymj] *arg=%d\n",*(long *)arg);
printk("[ymj] phy_addr arg=%x\n",virt_to_phys(arg));
printk("[ymj] phy arg=%d\n",*(long *)phys_to_virt(virt_to_phys(arg)));



    /* 检测命令的有效性 */
    if (_IOC_TYPE(cmd) != MEMDEV_IOC_MAGIC) 
        return -EINVAL;
    if (_IOC_NR(cmd) > MEMDEV_IOC_MAXNR) 
        return -EINVAL;


    /* 根据命令类型，检测参数空间是否可以访问 */
    if (_IOC_DIR(cmd) & _IOC_READ)
        err = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
        err = !access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd));
    if (err) 
        return -EFAULT;
    /* 根据命令，执行相应的操作 */
    switch(cmd) {

      /* 打印当前设备信息 */
      case MEMDEV_IOCPRINT:
          printk("<--- CMD MEMDEV_IOCPRINT Done--->\n\n");
        break;
      
      /* 获取参数 */
      case MEMDEV_IOCGETDATA: 
        ioarg = 1101;
        ret = __put_user(ioarg, (int *)arg);
        break;
      
      /* 设置参数 */
      case MEMDEV_IOCSETDATA: 
        ret = __get_user(ioarg, (int *)arg);
        printk("<--- In Kernel MEMDEV_IOCSETDATA ioarg = %d --->\n\n",ioarg);
        break;

      default:  
          printk("<--- 000000000000--->\n\n");
        return -EINVAL;
    }
    return ret;

 }  
 static struct file_operations hello_fops = {  
     .owner   =   THIS_MODULE,  
     .open    =   hello_open,  
     .release =   hello_close,   
     .read    =   hello_read,
     .write   =   hello_write,
     .unlocked_ioctl   =   hello_ioctl,
 };  

 

static int __init hello_init (void)
{
    int result;
    devno = MKDEV(HELLO_MAJOR, HELLO_MINOR);
    if (HELLO_MAJOR)
        result = register_chrdev_region(devno, NUMBER_OF_DEVICES, "memdev");
    else
    {
        result = alloc_chrdev_region(&devno, 0, NUMBER_OF_DEVICES, "memdev");
        HELLO_MAJOR = MAJOR(devno);
    }  
    printk("MAJOR IS %d\n",HELLO_MAJOR);
    my_class = class_create(THIS_MODULE,"hello_char_class");  //类名为hello_char_class
    if(IS_ERR(my_class)) 
    {
        printk("Err: failed in creating class.\n");
        return -1; 
    }
    device_create(my_class,NULL,devno,NULL,"memdev");      //设备名为memdev
    if (result<0) 
    {
        printk (KERN_WARNING "hello: can't get major number %d\n", HELLO_MAJOR);
        return result;
    }
 

    cdev_init(&cdev, &hello_fops);
    cdev.owner = THIS_MODULE;
    cdev_add(&cdev, devno, NUMBER_OF_DEVICES);
    printk (KERN_ERR "Character driver Registered\n");
    return 0;

}

 

static void __exit hello_exit (void)
{
    cdev_del (&cdev);
    device_destroy(my_class, devno);         //delete device node under /dev//必须先删除设备，再删除class类
    class_destroy(my_class);                 //delete class created by us
    unregister_chrdev_region (devno,NUMBER_OF_DEVICES);
    printk (KERN_INFO "char driver cleaned up\n");
}


module_init (hello_init);
module_exit (hello_exit);
MODULE_LICENSE ("GPL"); 
