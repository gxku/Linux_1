#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/pinctrl/consumer.h>
#include <linux/pwm.h>

#define HIGH	1
#define LOW	0

struct dr8890pwm_gpio_platform_data {
	int pwm_gpio[4];
	
	struct pinctrl			*pinctrl;
	struct pinctrl_state	*pwm_on[4];
	struct pinctrl_state	*pwm_off;
	
	struct pwm_device *pwm[4];
	int period_ns;
	int duty_ns;
};

struct dr8890pwm_gpio_platform_data pd;
struct dr8890pwm_gpio_platform_data *d = &pd;

static int tout_gpio_test(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	char tmpbuf[32];
	int ret, i;
	int level = 0;

	/*configure gpio*/

	for (i = 0; i < 4; i++)
	{
		sprintf(tmpbuf, "pwm%d-gpio", i);
		
		pd.pwm_gpio[i] = of_get_named_gpio(np, tmpbuf, 0);

		if (!gpio_is_valid(pd.pwm_gpio[i])) {
			printk(KERN_ERR "%s is invalid\n", tmpbuf);
			return -EINVAL;
		}
		
		ret = devm_gpio_request_one(&pdev->dev, pd.pwm_gpio[i], GPIOF_OUT_INIT_LOW, tmpbuf);
		if (ret)
			goto err;
		
		printk(KERN_INFO "%s ready!\n", tmpbuf);
	}	
		
	
	while (1)
	{
		pr_err("->%d\n", level);
		for (i = 0; i < 4; i++) {
			gpio_set_value(pd.pwm_gpio[i], level);
		}
		level = !level;
		
		usleep_range(1000000, 1000100);
	}

	return 0;
err:
	return -1;
}


 /**use this to set period
  * pwm_config() - change a PWM device configuration
  * @pwm: PWM device
  * @duty_ns: "on" time (in nanoseconds)
  * @period_ns: duration (in nanoseconds) of one cycle
  */
// int pwm_config(struct pwm_device *pwm, int duty_ns, int period_ns)


static int dr8890pwm_gpio_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	int ret, i;
	unsigned int ch_id;
	
	//tout_gpio_test(pdev);

	d->pinctrl = devm_pinctrl_get(dev);

	if (IS_ERR(d->pinctrl)) {
		printk("failed to get pinctrl\n");
		d->pinctrl = NULL;
		return -1;
	}

	for (i = 0; i < 4; i++) {
		char pinctrl_name[32];
		
		sprintf(pinctrl_name, "pwm%d_on", i);
		d->pwm_on[i] = pinctrl_lookup_state(d->pinctrl, pinctrl_name);
		if (IS_ERR(d->pwm_on)) {
			printk("failed to get pwm_on pin state %d\n", i);
			d->pwm_on[i] = NULL;
			return -1;
		}
		
		pinctrl_select_state(d->pinctrl, d->pwm_on[i]);
		printk(KERN_INFO "!!!PWM%d TOUT Ready\n", i);
	}
	
	for (ch_id = 0; ch_id < 4; ch_id++)
	{
		char pwm_label[32];
		
		sprintf(pwm_label, "pwm%d", ch_id);
		
		d->pwm[ch_id] = pwm_request(ch_id, pwm_label);
		if (IS_ERR(d->pwm)) {
			dev_err(dev, "%s: pwm request failed\n", __func__);
			ret = PTR_ERR(d->pwm[ch_id]);
			goto err;
		}
	
		ret = pwm_config(d->pwm[ch_id], 100000 * (ch_id + 1), 1000000);
		if (ret < 0) {
			dev_err(dev, "%s: pwm_config fail\n", __func__);
			goto err;
		}

		ret = pwm_enable(d->pwm[ch_id]);
		if (ret < 0) {
			dev_err(dev, "%s: pwm_enable fail\n", __func__);
			goto err;
		}
	}
	
	return 0;
err:
	return -1;
}

static int dr8890pwm_gpio_remove(struct platform_device *pdev)
{
	int i;
	
	for (i = 0; i < 4; i++)
	{
		pwm_disable(d->pwm[i]);
		pwm_free(d->pwm[i]);
	}

	return 0;
}

static const struct of_device_id dr8890pwm_gpio_of_match[] = {
	{.compatible = "dr8890pwm,gpio",},
	{},
};
MODULE_DEVICE_TABLE(of, dr8890pwm_gpio_of_match);

static struct platform_driver dr8890pwm_gpio_driver = {
	.probe  = dr8890pwm_gpio_probe,
	.remove = dr8890pwm_gpio_remove,
	.driver = {
		.owner = THIS_MODULE,
		.name = "dr8890pwm_gpio",
		.of_match_table = dr8890pwm_gpio_of_match,
	},
};

static int __init dr8890pwm_gpio_init(void)
{
	return platform_driver_register(&dr8890pwm_gpio_driver);
}

static void __exit dr8890pwm_gpio_exit(void)
{
	return platform_driver_unregister(&dr8890pwm_gpio_driver);
}

module_init(dr8890pwm_gpio_init);
module_exit(dr8890pwm_gpio_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nero xue");
