#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/timekeeping.h>
#include <linux/delay.h>

#include "i2c-exynos5.h"
/** i2c-IO-EXT_I2C0_XXX_1_8V
* hsi2c_14-14E50000
* reg = <0x0 0x14E50000 0x1000>
* irq = <0 444 0>
* sda = gpio : gpe6 2 0x1
* scl = gpio : gpe6 3 0x1
* clock = rate : 257, gate : 257
**/

#define GYPO_DRV_NAME "gyro-i2c"

struct exynos5_i2c *i2c;
struct i2c_adapter *gyro_adapter;
static unsigned short gyro_addr;
static struct of_device_id gyro_match_table[] = {
	{ .compatible = "mpu9250,gyro-i2c", },
	{},
};

static const struct i2c_device_id gyro_device_id[] = {
	{GYPO_DRV_NAME, 0},
	{}
};

#if 1 
int gyro_read_reg(unsigned char reg_addr, unsigned char *reg_data)
{
	struct i2c_msg xfer_msg[2];
	int ret;

	*reg_data = 0x0;

	//no lock test
	xfer_msg[0].addr = gyro_addr;
	xfer_msg[0].len = 1;
	xfer_msg[0].flags = 0;//wd
	xfer_msg[0].buf = &reg_addr;

	xfer_msg[1].addr = gyro_addr;
	xfer_msg[1].len = 1;
	xfer_msg[1].flags = I2C_M_RD;	
	xfer_msg[1].buf = reg_data;
	ret = i2c_transfer(gyro_adapter, xfer_msg, 2);

	//printk("read %d, reg 0x%x, return data 0x%x\n",ret,reg_addr,*reg_data);
	return ret;
}

int gyro_write_reg(unsigned char reg_addr, unsigned char reg_data)
{
	struct i2c_msg xfer_msg[2];
	int ret;

	uint8_t regbuf[2];
	
	//no lock test
	regbuf[0] = reg_addr;
	regbuf[1] = reg_data;

	xfer_msg[0].addr = gyro_addr;
	xfer_msg[0].len = 2; //Bytes
	xfer_msg[0].flags = 0;
	xfer_msg[0].buf = regbuf;

	ret = i2c_transfer(gyro_adapter, xfer_msg, 1);

	//printk("write %d, reg 0x%x , data 0x%x\n",ret, reg_addr,reg_data);
	return ret;
}

#define PWR_MGMT_1 		0x6b
#define SMPLRT_DIV 		0x19
#define CONFIG_CFG 		0x1a
#define USER_CONTROL 	0x6a
#define GYRO_CONFIG  	0x1b
#define ACCEL_CONFIG	0x1c
#define WHO_AM_I		0x75

static void init_mpu9250_test(void)
{
	uint8_t wmi;
	gyro_write_reg(PWR_MGMT_1, 0x0);
	gyro_write_reg(SMPLRT_DIV, 0x7);
	gyro_write_reg(CONFIG_CFG, 0x6);
	gyro_write_reg(USER_CONTROL, 0x0);
	gyro_write_reg(GYRO_CONFIG, 0x18);
	gyro_write_reg(ACCEL_CONFIG, 0x1);
	gyro_read_reg(WHO_AM_I,&wmi); 
	printk("gyro I am 0x%x\n",wmi);
}
#endif

static void do_test(void)
{
	int i;
	struct i2c_msg xfer_msg[2];
	char regbuf[512]= {0};
	regbuf[0] = (gyro_addr<<1|0);
	for (i=1;i<512;i++){
		regbuf[i] = i; 
	}

	xfer_msg[0].addr = gyro_addr;
	xfer_msg[0].len = 512; //Bytes
	xfer_msg[0].flags = 0;
	xfer_msg[0].buf = regbuf;
	i2c_transfer(gyro_adapter, xfer_msg, 1);
}

static struct timeval tv1[10],tv2[10];
static int sec[10], usec[10];
static int tsec, tusec;
static void bp_write_test(void)
{
	int i;
	tsec = 0;
	tusec = 0;
	for (i=0 ;i < 10; i++){
		do_gettimeofday(&tv1[i]);
		do_test();
		do_gettimeofday(&tv2[i]);

		sec[i] = tv2[i].tv_sec-tv1[i].tv_sec;
		usec[i] = tv2[i].tv_usec-tv1[i].tv_usec;
		printk("%d - Time: %d s, % d us\n",i, sec[i], usec[i]);
		tsec += sec[i];
		tusec += usec[i];
		mdelay(1000);
	}
	printk("Data 5120B, Time %d , %d: \n",tsec,tusec);
}

static int gyro_probe(struct i2c_client *client, const struct i2c_device_id *idp)
{
	//i2c = (struct exynos5_i2c *)client->adapter->algo_data;
	gyro_adapter = client->adapter;
	gyro_addr = client->addr; 
	//printk("gyro i2c probe on %s-%s,flags 0x%x, addr 0x%x, irq %d\n",
	//		client->adapter->name,dev_name(i2c->dev), 
	//		client->flags, client->addr, client->irq);
	printk("bp board addr 0x%x\n",gyro_addr);
	init_mpu9250_test();
	//bp_write_test();
	return 0;
}

static int gyro_remove(struct i2c_client *client)
{
	printk("gyro i2c remove\n");
	return 0;
}

static struct i2c_driver gyro_i2c_driver = {
	.driver = {
		.name = GYPO_DRV_NAME,
		.owner = THIS_MODULE,
		.of_match_table = gyro_match_table,
	},
	.probe = gyro_probe,
	.remove = gyro_remove,

	.id_table = gyro_device_id,	
};

static int __init gyro_init(void)
{
	int ret;
	ret = i2c_add_driver(&gyro_i2c_driver);//probe
	return 0;
}

static void __exit gyro_exit(void)
{
	i2c_del_driver(&gyro_i2c_driver);//remove
}

MODULE_LICENSE("GPL");
module_init(gyro_init);
module_exit(gyro_exit);
