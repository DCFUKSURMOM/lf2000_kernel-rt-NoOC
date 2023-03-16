/*
 * Neonode zForce touchscreen (I2C bus)
 *
 * Copyright (C) 2012 LeapFrog Enterprises, Inc.
 *
 * Licensed under the GPL-2 or later.
 */
 
/* SP: 02-22-13 -: Merged this driver code with zforce.c, not being compiled now. */
#if 0

#include <linux/input.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/types.h>

#include "zforce.h"

#ifdef CONFIG_PM
static int zforce_i2c_suspend(struct i2c_client *client, pm_message_t message)
{
	struct zforce *zf = i2c_get_clientdata(client);

	return zforce_suspend(zf);
}

static int zforce_i2c_resume(struct i2c_client *client)
{
	struct zforce *zf = i2c_get_clientdata(client);

	return zforce_resume(zf);
}
#else
# define zforce_i2c_suspend NULL
# define zforce_i2c_resume  NULL
#endif

static int zforce_i2c_read(const struct device *dev, u8 *val, unsigned int count)
{
	struct i2c_client *client = to_i2c_client(dev);
	s32 tmp;

	tmp = i2c_master_recv(client, val, count);

	if(likely((tmp == count)))
		return 0;

	if(tmp > 0)
		return -EIO;

	return tmp;
}

static int zforce_i2c_write(const struct device *dev, u8 *val, unsigned int count)
{
	struct i2c_client *client = to_i2c_client(dev);
	s32 tmp;

	tmp = i2c_master_send(client, val, count);

	if(likely((tmp == count)))
		return 0;

	if(tmp > 0)
		return -EIO;

	return tmp;

}

static const struct zforce_bus_ops zforce_i2c_bus_ops = {
	.bustype	= BUS_I2C,
	.read		= zforce_i2c_read,
	.write		= zforce_i2c_write,
};

static int __devinit zforce_i2c_probe(struct i2c_client *client,
				      const struct i2c_device_id *id)
{
	int ret;
	struct zforce *zf;
	printk(KERN_INFO "zforce_i2c_probe \n");

	if(!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE))
	{
		dev_err(&client->dev, "Incompatible i2c host\n");
		return -EIO;
	}

	if(0 != (ret = zforce_probe(&client->dev, client->irq,
			  &zforce_i2c_bus_ops, &zf)))
		return ret;

	i2c_set_clientdata(client, zf);

	return 0;
}

static int __devexit zforce_i2c_remove(struct i2c_client *client)
{
	struct zforce *zf = i2c_get_clientdata(client);

	return zforce_remove(zf, client->irq);
}

static const struct i2c_device_id zforce_i2c_id[] = {
	{ "zforce", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, zforce_i2c_id);

static struct i2c_driver zforce_i2c_driver = {
	.driver = {
		.name	= "zforce",
		.owner	= THIS_MODULE,
	},
	.probe		= zforce_i2c_probe,
	.remove		= __devexit_p(zforce_i2c_remove),
	.suspend	= zforce_i2c_suspend,
	.resume		= zforce_i2c_resume,
	.id_table	= zforce_i2c_id,
};

static int __init zforce_i2c_init(void)
{
	printk(KERN_INFO "zforce_i2c_init \n");
	return i2c_add_driver(&zforce_i2c_driver);
}
module_init(zforce_i2c_init);

static void __exit zforce_i2c_exit(void)
{
	i2c_del_driver(&zforce_i2c_driver);
}
module_exit(zforce_i2c_exit);

MODULE_AUTHOR("Justin Eno <jeno@leapfrog.com>");
MODULE_DESCRIPTION("Neonode zForce touchscreen I2C bus driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("i2c:zforce");

#endif
