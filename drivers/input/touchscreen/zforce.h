/*
 * Neonode zForce touchscreen (bus interfaces)
 *
 * Copyright (C) 2012 LeapFrog Enterprises, Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#ifndef _ZFORCE_H_
#define _ZFORCE_H_

#include <linux/types.h>
#include <linux/device.h>
#include <linux/i2c.h>

/* Debug Zforce */
//#define ZFORCE_DEBUG 1

/* Enable the following #define if you want to collect ISR timing data*/
//#define ZFORCE_TIMING 1

struct zforce;

struct zforce_bus_ops {
	u16 bustype;
	int (* const read)(const struct device *dev, u8 *val, unsigned int count);
	int (* const write)(const struct device *dev, u8 *val, unsigned int count);
};

int zforce_suspend(struct zforce *);
int zforce_resume(struct zforce *);
int zforce_probe(struct device *dev, unsigned int irq, const struct zforce_bus_ops *ops);
int zforce_remove(struct zforce *priv, unsigned int irq);

#endif /* _ZFORCE_H_ */
