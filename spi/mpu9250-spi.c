#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/spi/spi.h>

/** spi-IO-EXT_SPI
* spi_6-14da0000
* reg = <0x0 0x14da0000 0x100>
* irq = <0 454 0>
* clock = spi : 274, spi_busclk0 : 282
**/

#define GYPO_DRV_NAME "gyro-spi"

static unsigned short gyro_addr;
static struct of_device_id gyro_match_table[] = {
	{ .compatible = "mpu9250,gyro-spi", },
	{},
};

static const struct spi_device_id gyro_device_id[] = {
	{GYPO_DRV_NAME, 0},
	{}
};

#define PWR_MGMT_1              0x6b
#define SMPLRT_DIV              0x19
#define CONFIG_CFG              0x1a
#define USER_CONTROL    0x6a
#define GYRO_CONFIG     0x1b
#define ACCEL_CONFIG    0x1c
#define WHO_AM_I                0x75


static int mpu_write_reg(struct spi_device *spi, uint8_t reg, uint8_t dat)
{
    uint8_t buf[2] = {reg, dat};

    return spi_write_then_read(spi, &buf, 2, NULL, 0);
}

static int mpu_read_reg(struct spi_device *spi, uint8_t reg, uint8_t *dat)
{
   /* Set MSB to indicate read */ 
    uint8_t wb = reg | 0x80;

    return spi_write_then_read(spi, &wb, 1, dat, 1);
}

static void init_mpu9250_test(struct spi_device *spi)
{
    int ret;
    uint8_t wmi;

    mpu_write_reg(spi, PWR_MGMT_1, 0x0);
    mpu_write_reg(spi, SMPLRT_DIV, 0x7);
    mpu_write_reg(spi, CONFIG_CFG, 0x6);
    mpu_write_reg(spi, USER_CONTROL, 0x0);
    mpu_write_reg(spi, GYRO_CONFIG, 0x18);
    mpu_write_reg(spi, ACCEL_CONFIG, 0x1);
    mpu_read_reg(spi, WHO_AM_I,&wmi);
    printk(">>> GYRO WAMI = 0x%x\n", wmi);
}



static int gyro_probe(struct spi_device *spi)
{
	//i2c = (struct exynos5_i2c *)client->adapter->algo_data;
	gyro_addr = spi->chip_select; 
	//printk("gyro i2c probe on %s-%s,flags 0x%x, addr 0x%x, irq %d\n",
	//		client->adapter->name,dev_name(i2c->dev), 
	//		client->flags, client->addr, client->irq);
	printk("gyro-spi reg : 0x%x\n",gyro_addr);
	init_mpu9250_test(spi);
	return 0;
}

static int gyro_remove(struct spi_device *spi)
{
	printk("gyro i2c remove\n");
	return 0;
}

static struct spi_driver gyro_spi_driver = {
	.driver = {
		.name = GYPO_DRV_NAME,
		.owner = THIS_MODULE,
		.of_match_table = gyro_match_table,
	},
	.probe = gyro_probe,
	.remove = gyro_remove,

	.id_table = gyro_device_id,	
};

module_spi_driver(gyro_spi_driver);

MODULE_LICENSE("GPL");
