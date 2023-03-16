/*
 *  Neonode zForce touchscreen driver
 *
 *  Author:	Justin Eno, <jeno@leapfrog.com>, Sukhada Palav, <spalav@leapfrog.com>
 *  Created:	March 16, 2012
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Multi-bus structure based on ad7879.
 *
 * Implements the type B multi-touch protocol.
 */

/*
 * This is a front-end driver for Neonode zForce touchscreens.  It interfaces
 * with i2c, spi, and USB back-ends.
 */

#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/input.h>
#include <linux/module.h>
#include <linux/version.h>
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,37))
#include <linux/input/mt.h>
#endif
#include <linux/sched.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/lf2000/gpio.h>
#include <linux/sysfs.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>
#include <asm/system_info.h>

/* nexell soc headers */
#include <mach/soc.h>
#include <mach/board_revisions.h>

#include "zforce.h"


/*
 * This driver adheres to revision 4.3 of the "Hardware and Software Interface"
 * specification.
 */

/* Command and response identifiers */
enum zforce_cmd {
	DEACTIVATE		= 0x00,
	ACTIVATE		= 0x01,
	SETRESOLUTION		= 0x02,
	SETCONFIGURATION	= 0x03,
	TOUCHDATAREQUEST	= 0x04,
	BOOTCOMPLETE		= 0x07,
	SETSCANNINGFREQUENCY	= 0x08,
	SETTOUCHAREASIZES		= 0x09,
	LOWSIGNALREQUEST	= 0x0D,
	FIXEDPULSESTRENGTH	= 0x0F,
	//FORCECALIBRATION	= 0x1A,
	LEDLEVELREQUEST		= 0x1C,
	SETACTIVELEDS		= 0x1D,
	STATUSCOMMAND		= 0x1E,
	FORCEDLEDLEVELREQUEST	= 0x20,
	OPENSHORTREQUEST	= 0x21,
	//SETGPIOCONTROL		= 0x23, // Removed in version 4.3
	//GETGPIOCONTROL		= 0x24,
	OVERRUN			= 0x25,
	RUNPROXIMITY	= 0X26,
	PRODUCTIDREQUEST = 0X29,

	INVALID_CMD		= 0xFE,
};

/* Sparse array - used to minimize cmd_recv[] and cmd_data[] */
static const unsigned int cmd_to_idx[] = {
	/* Reserve index 0 for uninitialized (unknown) responses */
	[DEACTIVATE]		= 1,
	[ACTIVATE]		= 2,
	[SETRESOLUTION]		= 3,
	[SETCONFIGURATION]	= 4,
	[TOUCHDATAREQUEST]	= 5,
	[BOOTCOMPLETE]		= 6,
	[SETSCANNINGFREQUENCY]	= 7,
	[SETTOUCHAREASIZES]	= 8,
	[LOWSIGNALREQUEST]	= 9,
	[FIXEDPULSESTRENGTH]	= 10,
	//[FORCECALIBRATION]	= 11,
	[LEDLEVELREQUEST]	= 12,
	[SETACTIVELEDS]		= 13,
	[STATUSCOMMAND]		= 14,
	[FORCEDLEDLEVELREQUEST]	= 15,
	[OPENSHORTREQUEST]	= 16,
	//[SETGPIOCONTROL]	= 17,
	//[GETGPIOCONTROL]	= 18,
	[OVERRUN]		= 17,
	[RUNPROXIMITY]  = 18,
	[PRODUCTIDREQUEST] = 19,
};

// Command packet structures
// Not defining single byte structures like axis_info, port_info and run_proximity from table 1.1

struct size_data {
	u16	width;
	u16	height;
};

struct config_data {
	u32	flags;
};

struct scan_freq_data {
	u16	idle_freq;
	u16	finger_freq;
	u16	stylus_freq;
};

//struct touch_area_sizes {
	//touch_area_data max; // Largest touch object allowed 
	//touch_area_data min; // Smallest touch object allowed
//};

// Maximum and minimum size of an object that generates a touch
struct touch_area_data {
	u8	max_enabled;
	u8	max_size;
	u8	min_enabled;
	u8	min_size;
};

struct fixed_pulse_info {
	u8 axis; //X=0, Y=1
	u8 strength; //ASIC(1-64), Discrete(1-90)
};

struct active_leds {
	u8	x_start;
	u8	x_end;
	u8	y_start;
	u8	y_end;
};

//struct zforce_gpio_info {
	//u8 port; // 1-4 (P1OUT-P4OUT)
	//u8 direction; // 1 = Output, 0 = Input
	//u8 state; // 1 = High, 0 = Low, Don't care if direction == 0
//};

// Command response structures 
// Not defining low_signal_request, fixed_pulse_strength, 
// led_levels, open_short_data, proximity_response

struct touch_info {
	u16	x;
	u16	y;
	u8	id:6;
	u8	state:2;
	u8	width; //mm, 0-255
	u8	height; //mm, 0-255
	u8	pressure; // 0-255
	u8	probability; // 0-100
};

struct settings_data {
	u16	fw_major;
	u16	fw_minor;
	u16	fw_build;
	u16	fw_revision;
	u8	active:1;
	u8	touch_count:7;
	u16	scan_counter;
	u16	prepared_touch_pkgs;
	u16	sent_touch_pkgs;
	u16	invalid_touch_counter;

	struct config_data	config;

	struct scan_freq_data	freq_data;

	struct size_data	res;

	struct size_data	phys;

	struct active_leds	leds;
	
	u8	max_strength;

	struct touch_area_data	touch_size;

//	u8	reserved[84];
};

enum tri_state {
	EXPECTED	= -1,
	UNINITIALIZED	= 0,
	RECEIVED	= 1,
};	

enum touch_state {
	DOWN = 0,
	MOVE = 1,
	UP = 2,
	INVALID = 3,
};

struct zforce {
	struct device	*dev;
	const struct zforce_bus_ops	*ops;
	struct settings_data settings;
	struct work_struct zforce_work;		/* task			*/
	struct workqueue_struct *zforce_tasks;	/* workqueue		*/
	
	int	irq;
	struct input_dev *input_dev;
	int	irq_line;
	enum tri_state cmd_recv[ARRAY_SIZE(cmd_to_idx)];
	u8 *cmd_data[ARRAY_SIZE(cmd_to_idx)];
	wait_queue_head_t wait;
	spinlock_t lock;

	// A scalar value for touch pressure
	int pressure_scalar;
	// Counter for BOOTCOMPLETE to keep track of Neonode resetting itself
	unsigned int reboot_count;
	
	//Variables to parse Neonode responses, moving them here for workqueue implementation.
	//FIXME: Is there a better way?
	u8 packet_data[0xFF];
	int packet_len;
	bool packet_throw;
	bool zforce_debug;
};

static struct zforce *zfp = NULL;

#define ZFORCE_FRAME_START	0xEE

/* TODO: tune this value */
/* History: set_resolution: 94.220 - 76.535 = 19.685 */
#define ZFORCE_WAIT_MS	600

#define ZFORCE_WAIT_BOOTCOMPLETE 100 // 100ms (approx. timing provided by Neonode is 50-60ms)


#define ZFORCE_TOUCH_PACKET_LENGTH 9

/* Zforce timing */

#ifdef ZFORCE_TIMING

U32	NX_TIMER_GetTimerCounter( U32 ModuleIndex );

static u32 start_time;
static void timer_start(void)
{
	start_time = NX_TIMER_GetTimerCounter(0);
}

/* stop the stopwatch, and return the time */
static u32 timer_stop(void)
{
	return NX_TIMER_GetTimerCounter(0) - start_time;
}

// Calculations in drivers/mtd/nand/nexell/nand.c
static unsigned long ticks_to_microsecs( unsigned long ticks ) 
{
	return (u32)(0.192 * ticks);
}

#endif // ZFORCE_TIMING

/* Static functions */

/* Forward declarations to make the compiler happy */
static int zforce_init(struct zforce *zf);
static int zforce_init_ops(struct zforce *zf);
static int zforce_get_touchdata(struct zforce *zf);

/* Command dispatcher, optionally waits for a response */
static int zforce_command(struct zforce *zf, u8 *packet,
	const unsigned int len, const u8 *resp)
{
	int ret;
	const enum zforce_cmd cmd = packet[2];
	const unsigned int idx = cmd_to_idx[cmd];
		
	if(zf->zforce_debug)
	{
		dev_info(zf->dev, "zforce_command: Command being sent 0x%hhX\n", cmd);
	}

	/* u8 packet[] = {foo, sizeof(packet) - 2, bar, ...}; */
	packet[1] = len - 2;

	zf->cmd_recv[idx] = EXPECTED;
	zf->cmd_data[idx] = resp;

	if(0 != (ret = zf->ops->write(zf->dev, packet, len)))
		goto out;

	if(resp == NULL)
		goto out;

	#ifdef ZFORCE_TIMING
		u32 ticks;
		timer_start();
	#endif
	while(zf->cmd_recv[idx] != RECEIVED)
	{
		msleep(1);
	}
	#ifdef ZFORCE_TIMING
		ticks = timer_stop();
		dev_info(zf->dev, "zforce_command: time taken %u by command 0x%hhX\n", ticks_to_microsecs(ticks), cmd);
	#endif
	
out: 
	//dev_info(zf->dev, "zforce_command: %d out\n", ret);
	return ret;
}

#if 0
/* Wrapper for simple 0-byte-param, 1-byte-response commands */
static int zforce_simple_command(struct zforce *zf, const enum zforce_cmd cmd)
{
	int ret;
	u8 packet[] = {ZFORCE_FRAME_START, 0, cmd};
	const unsigned int idx = cmd_to_idx[cmd];
	
	ret = zforce_command(zf, packet, sizeof(packet), &packet[0]);

	if(ret == 0)
		ret = (packet[0] == 0) ? 0 : -EIO;
	else dev_warn(zf->dev, "zforce_simple_command: command failed \n");

	zf->cmd_recv[idx] = UNINITIALIZED;
	zf->cmd_data[idx] = NULL;

	return ret;
}
#endif

static int zforce_deactivate(struct zforce *zf)
{
	//int ret = zforce_simple_command(zf, DEACTIVATE);
	int ret;
	u8 packet[] = {ZFORCE_FRAME_START, 0, DEACTIVATE};
	const unsigned int idx = cmd_to_idx[DEACTIVATE];
	
	ret = zforce_command(zf, packet, sizeof(packet), &packet[0]);	
	if (ret != 0) 
		dev_warn(zf->dev, "zforce_deactivate: simple command failed\n");
		
	zf->cmd_recv[idx] = UNINITIALIZED;
	zf->cmd_data[idx] = NULL;

	return ret;
}

static int zforce_activate(struct zforce *zf)
{
	//int ret = zforce_simple_command(zf, ACTIVATE);
	int ret;
	u8 packet[] = {ZFORCE_FRAME_START, 0, ACTIVATE};
	const unsigned int idx = cmd_to_idx[ACTIVATE];
	
	ret = zforce_command(zf, packet, sizeof(packet), &packet[0]);	
	if (ret != 0)
		dev_warn(zf->dev, "zforce_deactivate: simple command failed\n");
		
	zf->cmd_recv[idx] = UNINITIALIZED;
	zf->cmd_data[idx] = NULL;

	return ret;
}

static int zforce_get_status(struct zforce *zf, struct settings_data *settings)
{
	int ret;
	u8 packet[] = {ZFORCE_FRAME_START, 0, STATUSCOMMAND};
	const unsigned int idx = cmd_to_idx[STATUSCOMMAND];

	ret = zforce_command(zf, packet, sizeof(packet), settings);
	if (ret != 0)
		dev_warn(zf->dev, "zforce_get_status: simple command failed\n");
	
	zf->cmd_recv[idx] = UNINITIALIZED;
	zf->cmd_data[idx] = NULL;
		
	return ret;
}

static int zforce_get_touchdata(struct zforce *zf)
{
	int ret;
	u8 packet[] = {ZFORCE_FRAME_START, 0, TOUCHDATAREQUEST};

	/* No (immediate) response expected */
	ret = zforce_command(zf, packet, sizeof(packet), NULL);

	return ret;
}

static int zforce_set_resolution(struct zforce *zf, const struct size_data *res)
{
	int ret;
	u8 packet[7] = {ZFORCE_FRAME_START, 0, SETRESOLUTION};

	*((__le16*)&packet[3]) = cpu_to_le16(res->width);
	*((__le16*)&packet[5]) = cpu_to_le16(res->height);
	//dev_info(zf->dev, "zforce_set_resolution: %u %u\n", res->width, res->height);

	ret = zforce_command(zf, packet, sizeof(packet), &packet[0]);

	if(ret == 0)
		ret = (packet[0] == 0) ? 0 : -EIO;
	else dev_warn(zf->dev, "zforce_set_resolution: Resolution was not set correctly \n");

	zf->cmd_recv[cmd_to_idx[SETRESOLUTION]] = UNINITIALIZED;
	zf->cmd_data[cmd_to_idx[SETRESOLUTION]] = NULL;

	return ret;
}

static int zforce_set_scanning_frequency(struct zforce *zf, const struct scan_freq_data *freq)
{
	int ret;
	u8 packet[9] = {ZFORCE_FRAME_START, 0, SETSCANNINGFREQUENCY};
	*((__le16*)&packet[3]) = cpu_to_le16(freq->idle_freq);
	*((__le16*)&packet[5]) = cpu_to_le16(freq->finger_freq);
	*((__le16*)&packet[7]) = cpu_to_le16(freq->stylus_freq);
	
	ret = zforce_command(zf, packet, sizeof(packet), &packet[0]);
	
	if(ret == 0)
		ret = (packet[0] == 0) ? 0 : -EIO;
	else dev_warn(zf->dev, "zforce_set_scanning_frequency: Scanning frequency was not set correctly \n");
	
	zf->cmd_recv[cmd_to_idx[SETRESOLUTION]] = UNINITIALIZED;
	zf->cmd_data[cmd_to_idx[SETRESOLUTION]] = NULL;

	return ret;
}

static int zforce_set_touch_area(struct zforce *zf, const struct touch_area_data *sizes)
{
	int ret;
	u8 packet[7] = {ZFORCE_FRAME_START, 0, SETTOUCHAREASIZES};
	
	u16 max = 0, min = 0;
	//max = sizes->max_size;
	//min = sizes->min_size;
		
	max = (sizes->max_size << 8) | (sizes->max_enabled);
	min = (sizes->min_size << 8) | (sizes->min_enabled);

	*((__le16*)&packet[3]) = cpu_to_le16(max);
	*((__le16*)&packet[5]) = cpu_to_le16(min);
	
	ret = zforce_command(zf, packet, sizeof(packet), &packet[0]);
	
	if(ret == 0)
		ret = (packet[0] == 0) ? 0 : -EIO;
	else dev_warn(zf->dev, "zforce_set_touch_area: Touch area was not set correctly \n");
	
	zf->cmd_recv[cmd_to_idx[SETTOUCHAREASIZES]] = UNINITIALIZED;
	zf->cmd_data[cmd_to_idx[SETTOUCHAREASIZES]] = NULL;

	return ret;
}

static int zforce_set_multitouch(struct zforce *zf, u32 value)
{
	int ret;
	u32 config_flags;
	u8 packet[7] = {ZFORCE_FRAME_START, 0, SETCONFIGURATION};
	
	if(0 != (ret = zforce_get_status(zf, &zf->settings)))
		return ret;
	
	config_flags = zf->settings.config.flags;
		
	if(value)
	{
		config_flags = config_flags | (1 << 0);
	}
	else
	{
		config_flags = config_flags & ~(1 << 0);
	}
	
	*((__le32*)&packet[3]) = cpu_to_le32(config_flags);
	
	ret = zforce_command(zf, packet, sizeof(packet), &packet[0]);
	
	if(ret == 0)
		ret = (packet[0] == 0) ? 0 : -EIO;
	else dev_warn(zf->dev, "zforce_set_multitouch: Multitouch was not set correctly \n");
	
	zf->cmd_recv[cmd_to_idx[SETCONFIGURATION]] = UNINITIALIZED;
	zf->cmd_data[cmd_to_idx[SETCONFIGURATION]] = NULL;
	
	return ret;
}

/* Recovery routine for Neonode if it resets itself for any reason */
static void zforce_restart(struct zforce *zf)
{
	zforce_init(zf);
	zforce_init_ops(zf);
	zforce_get_touchdata(zf);
	
	/* BOOTCOMPLETE	may arrive if touchscreen crashes and Zforce resets itself 
	* Increment reboot count and be prepared to receive it again */
	zf->reboot_count++;
	zf->cmd_recv[cmd_to_idx[BOOTCOMPLETE]] = EXPECTED;
	if(zf->zforce_debug)
	{
		dev_info(zf->dev, "Zforce restarted and reboot count is %d\n", zf->reboot_count);
	}
}

static void parse_freq(struct scan_freq_data *freq, u16 *raw)
{
	freq->idle_freq	= raw[0];
	freq->finger_freq	= raw[1];
	freq->stylus_freq	= raw[2];
}

static void parse_touch_area(struct touch_area_data *touch_area, u8 *raw)
{
	touch_area->max_enabled = raw[0];
	touch_area->max_size = raw[1];
	touch_area->min_enabled = raw[2];
	touch_area->min_size = raw[3];
}

static void parse_leds(struct active_leds *leds, u8 *raw)
{
	leds->x_start	= raw[0];
	leds->x_end	= raw[1];
	leds->y_start	= raw[2];
	leds->y_end	= raw[3];
}

static void parse_res(struct size_data *res, u8 *raw)
{
	res->width	= le16_to_cpup(&raw[0]);
	res->height	= le16_to_cpup(&raw[2]);
}

static void parse_config(struct config_data *config, u8 *raw)
{
	config->flags = le32_to_cpup(&raw[0]);
}

/* Parse reponse to STATUSCOMMAND */
static void parse_status(struct settings_data *settings, u8 *raw)
{
	settings->fw_major		= le16_to_cpup(&raw[0]);
	settings->fw_minor		= le16_to_cpup(&raw[2]);
	settings->fw_build		= le16_to_cpup(&raw[4]);
	settings->fw_revision		= le16_to_cpup(&raw[6]);

	settings->active		= raw[8] & 0x1;
	settings->touch_count		= (raw[8] >> 1) & 0x7F;

	settings->scan_counter		= le16_to_cpup(&raw[9]);
	settings->prepared_touch_pkgs	= le16_to_cpup(&raw[11]);
	settings->sent_touch_pkgs	= le16_to_cpup(&raw[13]);
	settings->invalid_touch_counter	= le16_to_cpup(&raw[15]);

	raw += 17;
	parse_config(&settings->config, raw);
	raw += sizeof(settings->config);

	parse_freq(&settings->freq_data, raw);
	raw += sizeof(settings->freq_data);

	parse_res(&settings->res, raw);
	raw += sizeof(settings->res);

	parse_res(&settings->phys, raw);
	raw += sizeof(settings->phys);

	parse_leds(&settings->leds, raw);
	raw += sizeof(settings->leds);
	
	settings->max_strength = le16_to_cpup(&raw);
	raw += 1;

	parse_touch_area(&settings->touch_size, raw);
	raw += sizeof(settings->touch_size);
	
	//parse_max_touch_area(&settings->maxSize, raw);
	//raw += sizeof(settings->maxSize);

    //parse_min_touch_area(&settings->minSize, raw);
	//raw += sizeof(settings->minSize);
	
	settings->max_strength = *raw;
}

/* Parse reponse to TOUCHDATAREQUEST */
static void parse_touch(struct zforce *zf, struct touch_info *ti, u8 *raw)
{
	ti->x		= le16_to_cpup(&raw[0]);
	ti->y		= le16_to_cpup(&raw[2]);
	ti->id		= raw[4] >> 2;
	ti->state	= raw[4] & 0x3;
	ti->width	= raw[5];
	ti->height	= raw[6];
	ti->pressure	= raw[7];
	ti->probability	= raw[8];

	if(zf->zforce_debug)
	{
		//dev_info(zf->dev, "%u %u %u %u %u %u %u %u \n", raw[0], raw[1], raw[2], raw[3], raw[4], raw[5], raw[6], raw[7]);
		//dev_info(zf->dev, "%u %u %u %u %u %u %u %u \n", ti->x, ti->y, ti->id, ti->state, ti->width, ti->height, ti->pressure, ti->probability);
		//dev_info(zf->dev, "%u %u %u %u \n", ti->x, ti->y, ti->id, ti->state);	
	}
}

static void process_touch(struct zforce *zf, struct touch_info *ti)
{	
	static u8 last_id = 0;

	if(last_id != ti->id)
	{
		last_id = ti->id;
		input_mt_slot(zf->input_dev, last_id - 1);
	}

	switch(ti->state) {
		case DOWN:
			if(zf->zforce_debug)
			{
				dev_info(zf->dev, "DOWN: %u %u %u %u %u\n", ti->x, ti->y, ti->id, ti->width, ti->height);	
			}
			input_report_abs(zf->input_dev, ABS_MT_TRACKING_ID, ti->id);
			if(ti->id == 1)
			{
				input_report_key(zf->input_dev, BTN_TOUCH, 1);
				input_report_abs(zf->input_dev, ABS_PRESSURE, min(100,(zf->pressure_scalar)*(ti->width)));
			}
			break;
		case UP:
			if(zf->zforce_debug)
			{
				dev_info(zf->dev, "UP: %u %u %u %u %u\n", ti->x, ti->y, ti->id, ti->width, ti->height);	
			}
			input_report_abs(zf->input_dev, ABS_MT_TRACKING_ID, -1);
			if(ti->id == 1)
			{
				input_report_key(zf->input_dev, BTN_TOUCH, 0);
				input_report_abs(zf->input_dev, ABS_PRESSURE, 0);
			}
			last_id = 0;
			break;
		case MOVE:
			if(zf->zforce_debug)
			{
				dev_info(zf->dev, "MOVE: %u %u %u %u %u\n", ti->x, ti->y, ti->id, ti->width, ti->height);	
			}
			if(ti->id == 1)
			{
				input_report_abs(zf->input_dev, ABS_PRESSURE, min(100,(zf->pressure_scalar)*(ti->width)));
			}
			break;
		case INVALID:
			if(zf->zforce_debug)
			{
				dev_info(zf->dev, "INVALID: %u %u %u %u %u\n", ti->x, ti->y, ti->id, ti->width, ti->height);	
			}
			input_report_abs(zf->input_dev, ABS_MT_TRACKING_ID, -1);
			if(ti->id == 1)
			{
				input_report_key(zf->input_dev, BTN_TOUCH, 0);
				input_report_abs(zf->input_dev, ABS_PRESSURE, 0);
			}
			last_id = 0;
			break;
		default: 
			dev_warn(zf->dev, "Undefined touch state \n");
			break;
	}

	if(ti->id == 1) 
	{
		input_report_abs(zf->input_dev, ABS_X, ti->x);
		input_report_abs(zf->input_dev, ABS_Y, ti->y);
	}

	input_report_abs(zf->input_dev, ABS_MT_PRESSURE, min(100,(zf->pressure_scalar)*(ti->width)));
	input_report_abs(zf->input_dev, ABS_MT_POSITION_X, ti->x);
	input_report_abs(zf->input_dev, ABS_MT_POSITION_Y, ti->y);
	input_report_abs(zf->input_dev, ABS_MT_TOUCH_MAJOR, ti->height);
	input_report_abs(zf->input_dev, ABS_MT_WIDTH_MAJOR, ti->width);
}

/* Response parser - dispatches to per-type parsers */
/* Superseded by parse_data_task for workqueue implementation */
static int parse_data(struct zforce *zf, u8 *data, const u8 len)
{
	int ret = 0;
	u8 i, j, count, boot_status;
	enum zforce_cmd cmd;
	
	#ifdef ZFORCE_TIMING
	u32 ticks;
	// For ISR Profiling
	static long int sum[ARRAY_SIZE(cmd_to_idx)] = {0};
	static int N[ARRAY_SIZE(cmd_to_idx)] = {0};
	#endif
	
	#ifdef ZFORCE_TIMING
		//timer_start();
	#endif

	for(i = 0; i < len; i++)
	{
		cmd = data[i];

		if(zf->zforce_debug)
		{
			//dev_info(zf->dev, "parse_data: Command Parsed 0x%hhX \n", cmd);
		}
		
		/* Handle this first because it is not a valid array index */
		if(unlikely(cmd == INVALID_CMD))
		{
			dev_warn(zf->dev, "Invalid command: 0x%hhX\n", data[++i]);
			goto out;
		}
		
		if(unlikely(cmd == OVERRUN))
		{
			dev_warn(zf->dev, "Overrun detected \n");
			goto out;
		}

		if(unlikely(zf->cmd_recv[cmd_to_idx[cmd]] != EXPECTED))
		{
			dev_warn(zf->dev, "Unexpected response: 0x%hhX\n", cmd);
			goto out;
		}
		
		#ifdef ZFORCE_TIMING
			dev_info(zf->dev, "parse data: %d %ld \n", N[cmd_to_idx[cmd]], sum[cmd_to_idx[cmd]]);
		#endif

		switch(cmd) {
		case DEACTIVATE:
		case ACTIVATE:
		case SETRESOLUTION:
		case SETCONFIGURATION:
		case SETSCANNINGFREQUENCY:
		case SETTOUCHAREASIZES:
		//case FORCECALIBRATION:
		case SETACTIVELEDS:
			*zf->cmd_data[cmd_to_idx[cmd]] = data[++i];
			zf->cmd_recv[cmd_to_idx[cmd]] = RECEIVED;
			break;
		case BOOTCOMPLETE:
			boot_status = data[++i];
			if(boot_status & 1)
			{
				dev_info(zf->dev, "parse_data: Reboot failed.\n");
			}
			else
			{
				dev_info(zf->dev, "parse_data: Reboot successful.\n");
				zf->cmd_recv[cmd_to_idx[cmd]] = RECEIVED;
			}
			//if(zf->reboot_count > 1)
			//{
				//zforce_restart(zf);
			//}
			break;	
		case STATUSCOMMAND:
			parse_status((struct settings_data *)zf->cmd_data[cmd_to_idx[cmd]],
				&data[++i]);

			zf->cmd_recv[cmd_to_idx[cmd]] = RECEIVED;
			i += len;
			break;
		case TOUCHDATAREQUEST:
			count = data[++i];			
			for(j = 0; j < count; j++)
			{
				struct touch_info ti;
				parse_touch(zf, &ti, &data[i+1]);
				process_touch(zf, &ti);
				i += ZFORCE_TOUCH_PACKET_LENGTH;
			}
			input_sync(zf->input_dev);
			break;

		case FIXEDPULSESTRENGTH:
		case LEDLEVELREQUEST:
		case FORCEDLEDLEVELREQUEST:
		case LOWSIGNALREQUEST:
		case OPENSHORTREQUEST:
		case RUNPROXIMITY:
		case PRODUCTIDREQUEST:
			break;

		default:
			ret = -EINVAL;
			BUG();
			break;
		}
	}
	
	#ifdef ZFORCE_TIMING
		//ticks = timer_stop();
		N[cmd_to_idx[cmd]]++;
		//sum[cmd_to_idx[cmd]] += ticks;
		//dev_info(zf->dev, "%d \n", N[cmd_to_idx[cmd]]);
		//dev_info(zf->dev, "parse_data: %d %ld time taken %u \n", N[cmd_to_idx[cmd]], sum[cmd_to_idx[cmd]],
			//ticks_to_microsecs(sum[cmd_to_idx[cmd]]));
		//dev_info(zf->dev, "parse_data: %d %ld time taken %u \n", N[cmd_to_idx[cmd]], sum[cmd_to_idx[cmd]],
			//(sum[cmd_to_idx[cmd]]/N[cmd_to_idx[cmd]]));
	#endif
	
	//gpio_set_value_cansleep(TP_DEBUG_NEONODE, 0);
	
out:
	return ret;
}

static void parse_data_task(struct work_struct *work)
{
	u8 i, j, count;
	enum zforce_cmd cmd;

	unsigned long flags;
	
	#ifdef ZFORCE_TIMING
	u32 ticks;
	// For ISR Profiling
	static long int sum[ARRAY_SIZE(cmd_to_idx)] = {0};
	static int N[ARRAY_SIZE(cmd_to_idx)] = {0};
	#endif
	
	#ifdef ZFORCE_TIMING
		//dev_info(zfp->dev, "parse_data %u\n", len);
		//timer_start();
	#endif
	
	//disable_irq(zfp->irq);
	
	spin_lock_irqsave(&zfp->lock, flags);
	//dev_info(zfp->dev, "zforce_isr: %u %u\n", zfp->packet_len, zfp->packet_data[0]);	

	for(i = 0; i < zfp->packet_len; i++)
	{
		//dev_info(zfp->dev, "zforce_isr: i = %d \n", i);
		cmd = zfp->packet_data[i];

		if(zfp->zforce_debug)
		{
			dev_info(zfp->dev, "parse_data: Command Parsed 0x%hhX \n", cmd);
		}

		/* Handle this first because it is not a valid array index */
		if(unlikely(cmd == INVALID_CMD))
		{
			dev_warn(zfp->dev, "Invalid command: 0x%hhX\n", zfp->packet_data[++i]);
			zfp->packet_throw = 1;
		}
		
		if(unlikely(cmd == OVERRUN))
		{
			dev_warn(zfp->dev, "Overrun detected \n");
			zfp->packet_throw = 1;
		}

		if(unlikely(zfp->cmd_recv[cmd_to_idx[cmd]] != EXPECTED))
		{
			dev_warn(zfp->dev, "Unexpected response: 0x%hhX\n", cmd);
			zfp->packet_throw = 1;
		}

		#ifdef ZFORCE_TIMING
			dev_info(zfp->dev, "parse data: %d %ld \n", N[cmd_to_idx[cmd]], sum[cmd_to_idx[cmd]]);
		#endif
		
		//dev_info(zfp->dev, "zforce_isr: packet_throw = %d \n", zfp->packet_throw);
		if(zfp->packet_throw) continue;

		switch(cmd) {
		case DEACTIVATE:
		case ACTIVATE:
		case SETRESOLUTION:
		case SETCONFIGURATION:
		case SETSCANNINGFREQUENCY:
		case SETTOUCHAREASIZES:
		//case FORCECALIBRATION:
		case SETACTIVELEDS:
			i = i + 1;
			*zfp->cmd_data[cmd_to_idx[cmd]] = zfp->packet_data[i];
			zfp->cmd_recv[cmd_to_idx[cmd]] = RECEIVED;
			//dev_info(zfp->dev, "zforce_isr: i = %d \n", i);
			break;
		case BOOTCOMPLETE:
			i = i + 1;
			if(zfp->zforce_debug)
			{
				dev_info(zfp->dev, "parse_data: Reboot %s.\n",
							(zfp->packet_data[i] == 0) ? "successful" : "failed");
			}
			zfp->cmd_recv[cmd_to_idx[cmd]] = RECEIVED;
			//dev_info(zfp->dev, "zforce_isr: i = %d \n", i);
			break;	
		case STATUSCOMMAND:
			i = i + 1;
			parse_status((struct settings_data *)zfp->cmd_data[cmd_to_idx[cmd]],
				&zfp->packet_data[i]);
			zfp->cmd_recv[cmd_to_idx[cmd]] = RECEIVED;
			i += zfp->packet_len;			
			break;
		case TOUCHDATAREQUEST:
			count = zfp->packet_data[++i];			
			for(j = 0; j < count; j++)
			{
				struct touch_info ti;
				parse_touch(zfp, &ti, &zfp->packet_data[i+1]);
				process_touch(zfp, &ti);
				i += ZFORCE_TOUCH_PACKET_LENGTH;
			}
			input_sync(zfp->input_dev);
			break;

		case FIXEDPULSESTRENGTH:
		case LEDLEVELREQUEST:
		case FORCEDLEDLEVELREQUEST:
		case LOWSIGNALREQUEST:
		case OPENSHORTREQUEST:
		case RUNPROXIMITY:
		case PRODUCTIDREQUEST:
			break;

		default:
			BUG();
			break;
		}
	}
	
	#ifdef ZFORCE_TIMING
		ticks = timer_stop();
		N[cmd_to_idx[cmd]]++;
		sum[cmd_to_idx[cmd]] += ticks;
		//dev_info(zfp->dev, "%d \n", N[cmd_to_idx[cmd]]);
		dev_info(zfp->dev, "parse_data: %d %ld time taken %u \n", N[cmd_to_idx[cmd]], sum[cmd_to_idx[cmd]],
			ticks_to_microsecs(sum[cmd_to_idx[cmd]]));
		//dev_info(zf->dev, "parse_data: %d %ld time taken %u \n", N[cmd_to_idx[cmd]], sum[cmd_to_idx[cmd]],
			//(sum[cmd_to_idx[cmd]]/N[cmd_to_idx[cmd]]));
	#endif
	
	spin_unlock_irqrestore(&zfp->lock, flags);
	
	//gpio_set_value_cansleep(TP_DEBUG_NEONODE, 0);
	//enable_irq(zfp->irq);
}

static irqreturn_t zforce_isr(int irq, void *priv)
{
	struct zforce *zf = priv;
	int ret;
	u8 recv[2];
	u8 data[0xFF];	/* ReceiveData package must fit in 8-bits (0xFF) */

	//dev_info(zf->dev, "zforce_isr: Calling ISR\n");

	ret = zf->ops->read(zf->dev, recv, sizeof(recv));
	if(unlikely(0 != ret))
	{
		dev_warn(zf->dev, "Failed read\n");
		goto out;
	}

	if(unlikely(ZFORCE_FRAME_START != recv[0]))
	{
		dev_warn(zf->dev, "Bad packet: %#hhx\n", recv[0]);
		goto out;
	}
	else
	{
		//gpio_set_value_cansleep(TP_DEBUG_NEONODE, 1);
	}
	//dev_info(zf->dev, "zforce_isr: %u %u\n", recv[0], recv[1]);

	ret = zf->ops->read(zf->dev, data, recv[1]);
	if(unlikely(0 != ret))
	{
		dev_warn(zf->dev, "Failed read\n");
		goto out;
	}

	ret = parse_data(zf, data, recv[1]);
	if(unlikely(0 != ret))
		goto out;

	wake_up_interruptible(&zf->wait);

out:
	return IRQ_HANDLED;
}

static int zforce_select_irq(struct zforce *zf)
{
	int ret = 0;

	/* TODO: request at board or bus level? */
	switch(system_rev) {
		case LF2000_BOARD_RIO:
		case LF2000_BOARD_RIO_KND_800_480:
		case LF2000_BOARD_RIO_BETA_1024_600:
		case LF2000_BOARD_RIO_BETA_800_480:
		case LF2000_BOARD_RIO_BETA_1024_600_700_400:
		case LF2000_BOARD_RIO_BETA_800_480_700_400:
		case LF2000_BOARD_RIO_BETA_1024_600_550_275:
		case LF2000_BOARD_RIO_EP_550_275:
		case LF2000_BOARD_RIO_EP_666_333:
		case LF2000_BOARD_RIO_EP_800_333:
		case LF2000_BOARD_RIO_EP_700_400:
		case LF2000_BOARD_RIO_EP_800_400:
		case LF2000_BOARD_RIO_FEP_800_327P67:
		case LF2000_BOARD_RIO_FEP_800_327P666:
			zf->irq_line = TP_INT;
			break;
		case LF2000_BOARD_LUCY:
		case LF2000_BOARD_LUCY_PP:
		case LF2000_BOARD_LUCY_CIP:
		case LF2000_BOARD_VALENCIA:
		case LF2000_BOARD_VALENCIA_EP:
		case LF2000_BOARD_VALENCIA_EP_8:
		case LF2000_BOARD_VALENCIA_FEP:
		case LF2000_BOARD_VALENCIA_FEP_8:
		case LF2000_BOARD_VALENCIA_EP_800_480:
		case LF2000_BOARD_VALENCIA_EP_800_480_8:
		case LF2000_BOARD_VALENCIA_FEP_800_480:
		case LF2000_BOARD_VALENCIA_FEP_800_480_8:
		case LF2000_BOARD_VALENCIA_KND_800_480:
		case LF2000_BOARD_VALENCIA_KND_800_480_8:
		case LF2000_BOARD_VALENCIA_KND_1024_600:
		case LF2000_BOARD_VALENCIA_KND_1024_600_8:
		case LF2000_BOARD_VALENCIA_CIP:
		case LF2000_BOARD_VTK:
			zf->irq_line = (int)(zf->dev->platform_data);
			break;
 		default:
			printk(KERN_ERR "%s: Invalid Board Revision 0x%X \n", 
				__func__, system_rev);
			break;
	}

	ret = gpio_request_one(zf->irq_line, GPIOF_IN,	"zForce irq");
		
	return ret;
}

static int zforce_release_reset(void)
{
	int ret = 0;
	/* Set RESET high to start executing */
	switch(system_rev) {
		case LF2000_BOARD_RIO:
		case LF2000_BOARD_RIO_KND_800_480:		
			{
				if(0 != (ret = gpio_request_one(WIFI_RESET, GPIOF_OUT_INIT_HIGH, "Neonode Reset")))
					//dev_info(zf->dev, "gpio wifi %d\n", ret);
					ret = 1;
				break;
			}
		case LF2000_BOARD_RIO_BETA_1024_600:
		case LF2000_BOARD_RIO_BETA_800_480:
		case LF2000_BOARD_RIO_BETA_1024_600_700_400:
		case LF2000_BOARD_RIO_BETA_800_480_700_400:
		case LF2000_BOARD_RIO_BETA_1024_600_550_275:
		case LF2000_BOARD_RIO_EP_550_275:
		case LF2000_BOARD_RIO_EP_666_333:
		case LF2000_BOARD_RIO_EP_800_333:
		case LF2000_BOARD_RIO_EP_700_400:
		case LF2000_BOARD_RIO_EP_800_400:
		case LF2000_BOARD_RIO_FEP_800_327P67:
		case LF2000_BOARD_RIO_FEP_800_327P666:
			{		
				if(0 != (ret = gpio_request_one(NEONODE_TEST, GPIOF_OUT_INIT_LOW, "Neonode Test")))
					//dev_info(zf->dev, "gpio wifi %d\n", ret);
					ret = 1;
				if(0 != (ret = gpio_request_one(NEONODE_RST, GPIOF_OUT_INIT_HIGH, "Neonode Reset")))
					//dev_info(zf->dev, "gpio wifi %d\n", ret);
					ret = 1;
				break;
			}
		default:
			printk(KERN_ERR "%s: Invalid Board Revision 0x%X - Neonode not supported\n",
				__func__, system_rev);
			break;
	}
	
	return ret;
}

static int zforce_init(struct zforce *zf)
{
	int ret;
	struct size_data res;
	int width, height;
	//struct scan_freq_data frequency;
	//struct touch_area_data touch_sizes;
	
	// Activate - Must-do
	if(0 != (ret = zforce_activate(zf)))
		goto out;
	
	// SetResolution - Must-do
	// initialize to make compiler happy 
	res.width = 0;
	res.height = 0;
	soc_dpc_get_resolution(0, &width, &height);

	// check that width and height are within range
	if (0 <= width && width < 0xFFFF)
		res.width = width;
	else
		printk(KERN_ERR "%s width (0x%8.8X) out of range", __FUNCTION__, width);
	if ( 0 <= height && width < 0xFFFF)
		res.height = height;
	else
		printk(KERN_ERR "%s height (0x%8.8X) out of range", __FUNCTION__, height);

	//dev_info(zf->dev, "zforce_probe: width %u height %u\n", res.width, res.height);
	
	if(0 != (ret = zforce_set_resolution(zf, &res)))
		goto out;
		
out:
	return ret;
}

// A function to do optional set operations in probe,
// dump everything here.
static int zforce_init_ops(struct zforce *zf)
{
	int ret;
	struct scan_freq_data frequency;
	struct touch_area_data touch_sizes;
	
	// SetScanningFrequency - Optional
	frequency.idle_freq = 25;
	frequency.finger_freq = 50;
	frequency.stylus_freq = 50;
	
	if(0 != (ret = zforce_set_scanning_frequency(zf, &frequency)))
		goto out;
	
	// SetTouchSizeLimits - Optional
	touch_sizes.max_enabled = 1;
	touch_sizes.max_size = 20;
	touch_sizes.min_enabled = 0;
	touch_sizes.min_size = 0;
	
	if(0 != (ret = zforce_set_touch_area(zf, &touch_sizes)))
		goto out;
		
	//Turn off multi-touch by default
	if(0 != (ret = zforce_set_multitouch(zf, 0)))
		goto out;
	
out: 
	return ret;
}

static void zforce_reset(void)
{
	// Reset Neonode in HW
	gpio_set_value_cansleep(NEONODE_RST, 0);
	
	// Delay as recommended
	msleep(1);
	
	// Release the reset
	gpio_set_value_cansleep(NEONODE_RST, 1);
}

#if 0
static int zforce_open(struct input_dev *input_dev)
{
	struct zforce *zf = input_get_drvdata(input_dev);
	int ret;

	enable_irq(zf->irq);

	if(0 != (ret = zforce_activate(zf)))
		disable_irq(zf->irq);

	if(ret == 0)
		ret = zforce_get_touchdata(zf);

	return ret;
}

static void zforce_close(struct input_dev *input_dev)
{
	struct zforce *zf = input_get_drvdata(input_dev);

	zforce_deactivate(zf);
	disable_irq(zf->irq);
}
#endif

/* sysfs interface */

static ssize_t show_resolution(struct device *dev, struct device_attribute *attr, char* buf)
{
	struct zforce *zf = (struct zforce *)dev_get_drvdata(dev);

	return sprintf(buf, "%ux%u \n", zf->settings.res.width, zf->settings.res.height);
}

static DEVICE_ATTR(resolution, S_IRUSR|S_IRGRP|S_IROTH, show_resolution, NULL);

static ssize_t show_scanning_frequency(struct device *dev, struct device_attribute *attr, char* buf)
{
	int ret = 0;
	struct zforce *zf = (struct zforce *)dev_get_drvdata(dev);
	
	if(0 != (ret = zforce_get_status(zf, &zf->settings)))
		return ret;
	
	return sprintf(buf, "Idle: %u Finger: %u Stylus: %u\n", zf->settings.freq_data.idle_freq,
		zf->settings.freq_data.finger_freq, zf->settings.freq_data.stylus_freq);
}

static ssize_t set_scanning_frequency(struct device *dev, struct device_attribute *attr, const char* buf, size_t count)
{
	int ret = 0;

	struct zforce *zf = (struct zforce *)dev_get_drvdata(dev);
	struct scan_freq_data frequency;
	
	if(3 != sscanf(buf, "%hu %hu %hu", &frequency.idle_freq, &frequency.finger_freq, &frequency.stylus_freq))
		return -EINVAL;
	
	if(0 != (ret = zforce_set_scanning_frequency(zf, &frequency)))
		return ret;
		
	return count;
}

static DEVICE_ATTR(scanning_frequency, S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH, show_scanning_frequency, set_scanning_frequency);

static ssize_t show_max_touch_size(struct device *dev, struct device_attribute *attr, char* buf)
{
	int ret = 0;
	struct zforce *zf = (struct zforce *)dev_get_drvdata(dev);
	
	if(0 != (ret = zforce_get_status(zf, &zf->settings)))
		return ret;
	
	return sprintf(buf, "Touch size: %u \n", zf->settings.touch_size.max_size);
}

static ssize_t set_max_touch_size(struct device *dev, struct device_attribute *attr, const char* buf, size_t count)
{
	int ret = 0;
	
	struct zforce *zf = (struct zforce *)dev_get_drvdata(dev);
	struct touch_area_data touch_sizes;
	int max_size;
	
	if(1 != sscanf(buf, "%u", &max_size))
		return -EINVAL;

	//check range of max_size
	if (0 < max_size && max_size <= 127)
		touch_sizes.max_size = max_size;
	else
		printk(KERN_ERR "%s max_size (0x%8.8X) is out of range\n", __FUNCTION__, max_size);

	touch_sizes.max_enabled = 1;
	touch_sizes.min_enabled = 0;
	touch_sizes.min_size = 0;
	
	if(0 != (ret = zforce_set_touch_area(zf, &touch_sizes)))
		return ret;
	
	return count;
}

static DEVICE_ATTR(max_touch_size, S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH, show_max_touch_size, set_max_touch_size);

static ssize_t show_firmware_version(struct device *dev, struct device_attribute *attr, char* buf)
{
	struct zforce *zf = (struct zforce *)dev_get_drvdata(dev);
		
	return sprintf(buf, "%u.%u.%u.%u \n", zf->settings.fw_major, zf->settings.fw_minor, zf->settings.fw_build, zf->settings.fw_revision);
}

static DEVICE_ATTR(firmware_version, S_IRUSR|S_IRGRP|S_IROTH, show_firmware_version, NULL);

static ssize_t show_physical_sizes(struct device *dev, struct device_attribute *attr, char* buf)
{
	struct zforce *zf = (struct zforce *)dev_get_drvdata(dev);

	return sprintf(buf, "%umm by %umm \n", zf->settings.phys.width, zf->settings.phys.height);
}

static DEVICE_ATTR(physical_sizes, S_IRUSR|S_IRGRP|S_IROTH, show_physical_sizes, NULL);

static ssize_t show_pressure_scalar(struct device *dev, struct device_attribute *attr, char* buf)
{
	struct zforce *zf = (struct zforce *)dev_get_drvdata(dev);
	
	return sprintf(buf, "Pressure Scalar: %u \n", zf->pressure_scalar);
}

static ssize_t set_pressure_scalar(struct device *dev, struct device_attribute *attr, const char* buf, size_t count)
{
	int scale;
	struct zforce *zf = (struct zforce *)dev_get_drvdata(dev);

	if(1 != sscanf(buf, "%d", &scale))
		return -EINVAL;
	if (scale < 0)
		return -EINVAL;
		
	zf->pressure_scalar = scale;

	return count;
}

static DEVICE_ATTR(pressure_scalar, S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH, show_pressure_scalar, set_pressure_scalar);

static ssize_t show_all_settings(struct device *dev, struct device_attribute *attr, char* buf)
{
	int ret = 0;
	ssize_t count = 0;
	struct zforce *zf = (struct zforce *)dev_get_drvdata(dev);
	
	if(0 != (ret = zforce_get_status(zf, &zf->settings)))
		return ret;
	
	count = sprintf(buf, "Firmware Version: %u.%u.%u.%u \nIdle: %u Finger: %u Stylus: %u\nResolution: %ux%u \nPhysical Size:%u mm by %u mm \nX_LEDs: %u Y_LEDs: %u \nTouch size: %u \n",
	zf->settings.fw_major, zf->settings.fw_minor, zf->settings.fw_build, zf->settings.fw_revision,
	zf->settings.freq_data.idle_freq, zf->settings.freq_data.finger_freq, zf->settings.freq_data.stylus_freq, zf->settings.res.width, zf->settings.res.height,
	zf->settings.phys.width, zf->settings.phys.height, (zf->settings.leds.x_end - zf->settings.leds.x_start + 1),(zf->settings.leds.y_end - zf->settings.leds.y_start + 1),
	zf->settings.touch_size.max_size);
	
	return count;
}

static DEVICE_ATTR(all_settings, S_IRUSR|S_IRGRP|S_IROTH, show_all_settings, NULL);

static ssize_t show_multitouch(struct device *dev, struct device_attribute *attr, char* buf)
{
	int ret = 0;
	struct zforce *zf = (struct zforce *)dev_get_drvdata(dev);
	
	if(0 != (ret = zforce_get_status(zf, &zf->settings)))
		return ret;
		
	if(zf->settings.config.flags & (1 << 0))
		return sprintf(buf, "Multitouch Enabled\n");
	else
		return sprintf(buf, "Multitouch Disabled\n");
}

static ssize_t set_multitouch(struct device *dev, struct device_attribute *attr, const char* buf, size_t count)
{
	int ret = 0;
	u32 value;

	struct zforce *zf = (struct zforce *)dev_get_drvdata(dev);
	
	if(1 != sscanf(buf, "%u", &value))
		return -EINVAL;
	
	if(0 != (ret = zforce_set_multitouch(zf, value)))
		return ret;
		
	return count;
}

static DEVICE_ATTR(multitouch, S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH, show_multitouch, set_multitouch);

static ssize_t show_debug(struct device *dev, struct device_attribute *attr, char* buf)
{
	int ret = 0;
	struct zforce *zf = (struct zforce *)dev_get_drvdata(dev);
	
	if(zf->zforce_debug & (1 << 0))
		return sprintf(buf, "Debugging Enabled\n");
	else
		return sprintf(buf, "Debugging Disabled\n");
}

static ssize_t set_debug(struct device *dev, struct device_attribute *attr, const char* buf, size_t count)
{
	int ret = 0;
	u32 value;

	struct zforce *zf = (struct zforce *)dev_get_drvdata(dev);
	
	if(1 != sscanf(buf, "%u", &value))
		return -EINVAL;
	
	if(value)
		zf->zforce_debug = 1;
	else
		zf->zforce_debug = 0;
		
	return count;
}

static DEVICE_ATTR(debug, S_IRUSR|S_IRGRP|S_IROTH|S_IWUSR|S_IWGRP|S_IWOTH, show_debug, set_debug);

static struct attribute *zforce_attributes[] = {
	&dev_attr_resolution.attr,
	&dev_attr_scanning_frequency.attr,
	&dev_attr_max_touch_size.attr,
	&dev_attr_firmware_version.attr,
	&dev_attr_physical_sizes.attr,
	&dev_attr_pressure_scalar.attr,
	&dev_attr_all_settings.attr,
	&dev_attr_multitouch.attr,
	&dev_attr_debug.attr,
	//&dev_attr_product_id.attr,  //Get
	//&dev_attr_active_led.attr,
	//&dev_attr_low_signal.attr,
	//&dev_attr_fixed_signal.attr,
	//&dev_attr_led_levels.attr,
	//&dev_attr_forced_led_levels.attr,
	//&dev_attr_open_short.attr,
	NULL
};

static struct attribute_group zforce_attr_group = {
	.attrs = zforce_attributes
};

static int zforce_create_sysfs(struct zforce *zf)
{
	int err, ret = 0;
	struct kobject *i2c_kobj = (zf->dev->kobj).parent;
	struct kobject *nx_kobj = i2c_kobj->parent;
	struct kobject *plat_kobj = nx_kobj->parent;
		
	if(0 != (sysfs_create_group(&zf->dev->kobj, &zforce_attr_group)))
		ret = -ENOMEM;
	
	err = sysfs_create_link(plat_kobj, &zf->dev->kobj, "lf2000-touchscreen");
	if(err)
	{
		dev_info(zf->dev, "could not create device link from %s to %s err %d\n", plat_kobj->name,  (zf->dev->kobj).name, err);
		ret = err;
	}
	return ret;
}

static void zforce_delete_sysfs(struct zforce *zf)
{
	struct kobject *i2c_kobj = (zf->dev->kobj).parent;
	struct kobject *nx_kobj = i2c_kobj->parent;
	struct kobject *plat_kobj = nx_kobj->parent;

	sysfs_remove_group(&zf->dev->kobj, &zforce_attr_group);
	
	sysfs_remove_link(plat_kobj, "lf2000-touchscreen");
}

#ifdef CONFIG_PM
int zforce_suspend(struct zforce *zf)
{
	return zforce_deactivate(zf);
}
EXPORT_SYMBOL(zforce_suspend);

int zforce_resume(struct zforce *zf)
{
	return zforce_activate(zf);
}
EXPORT_SYMBOL(zforce_resume);
#endif /* CONFIG_PM */

int zforce_probe(struct device *dev, unsigned int irq,
		const struct zforce_bus_ops *ops)//, struct zforce **priv)
{
	
	int ret = -ENOMEM;
	int i, counter = 0;

	struct input_dev *input_dev;
	struct zforce *zf;
	
	input_dev = input_allocate_device();

	if(!input_dev) {
		ret = -ENOMEM;
		goto out_alloc;
	}

	zf = kzalloc(sizeof(struct zforce), GFP_KERNEL);
	if(!zf) {
		ret = -ENOMEM;
		printk(KERN_ERR "%s unable to allocate memory\n", __FUNCTION__);
		goto out;
	}

	zf->irq		= irq;
	zf->input_dev	= input_dev;
	zf->dev		= dev;
	zf->ops		= ops;
	zf->pressure_scalar = 5;
	zf->reboot_count = 1;
	zf->packet_throw = 0;
	zf->zforce_debug = 0;
	
	zfp = zf;

	/* zf->dev now valid, select an IRQ */
	if(0 != (ret = zforce_select_irq(zf)))
		goto out_gpio;

	init_waitqueue_head(&zf->wait);
	spin_lock_init(&zf->lock);
	
	/* initialize worker thread which processes irq */
	zf->zforce_tasks = create_singlethread_workqueue("neonode tasks");
	INIT_WORK(&zf->zforce_work, parse_data_task);
	
	/* Obtain a Debug GPIO */
	//if(0 != (ret = gpio_request_one(TP_DEBUG_NEONODE, GPIOF_OUT_INIT_LOW, "Neonode debug")))
		//dev_info(zf->dev, "gpio dbug %d\n", ret);
		//goto out_gpio;
	
	/* Delay to ensure Target Voltage level is reached */
	/* FIXME: Consult Sam on how much the delay should be */
	msleep(1);
		
	/* Set RESET high to start executing */
	if(0 != (ret = zforce_release_reset()))
		goto out_gpio;

	/* BOOTCOMPLETE	may arrive once IRQ is activated */
	zf->cmd_recv[cmd_to_idx[BOOTCOMPLETE]] = EXPECTED;

	/* Threaded IRQ because sleepy bus (i2c/spi) access is required */
	if(0 != (ret = request_threaded_irq(zf->irq, NULL, zforce_isr,
		IRQF_TRIGGER_LOW | IRQF_ONESHOT | IRQF_SAMPLE_RANDOM,
		dev_name(dev), zf)))
		goto out_irq;
	//else dev_info(zf->dev, "zforce_probe: %d \n", ret);
	
	// If BootComplete wasn't received in 100 ms, reset the controller
	// and try one more time. If BootComplete is still not received, 
	// print error message, do the clean-up and return from probe.
	for(i = 0; i <= 2; i++)
	{
		if(i == 2) goto out_bootfail;
		if(i == 1)
		{
			dev_info(zf->dev, "Resetting Zforce controller because BootComplete wasn't received \n");
			zforce_reset();
		}
		
		for(counter = 0; counter < ZFORCE_WAIT_BOOTCOMPLETE; counter++)
		{
			if(zf->cmd_recv[cmd_to_idx[BOOTCOMPLETE]] == RECEIVED)
			{
				counter = ZFORCE_WAIT_BOOTCOMPLETE + 1;
				///* BOOTCOMPLETE	may arrive if touchscreen crashes and Zforce resets itself 
				 //* Increment reboot count and be prepared to receive it again */
				//zf->reboot_count++;
				//zf->cmd_recv[cmd_to_idx[BOOTCOMPLETE]] = EXPECTED;
				
				break;
			}
			msleep(1);
		}
		
		if(counter > ZFORCE_WAIT_BOOTCOMPLETE) break;
	}

	dev_info(zf->dev, "Bootcomplete read and reboot count is %d\n", zf->reboot_count);

	/* BOOTCOMPLETE	may arrive if touchscreen crashes and Zforce resets itself */
	//zf->cmd_recv[cmd_to_idx[BOOTCOMPLETE]] = EXPECTED;

	/* TODO: Brio compatibility */
	input_dev->name		= "touchscreen interface";
//	input_dev->phys		=
//	input_dev->dev.parent	= dev
	input_dev->id.bustype	= ops->bustype;
//	input_dev->open		= zforce_open;
//	input_dev->close	= zforce_close;

	input_set_drvdata(input_dev, zf);

//	input_dev->evbit[0] = BIT(EV_KEY) | BIT(EV_ABS);
	__set_bit(EV_KEY, input_dev->evbit);
	__set_bit(EV_ABS, input_dev->evbit);
	input_dev->keybit[BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH);

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,37))
	if(0 != (ret = input_mt_init_slots(input_dev, 2))) {
#else
	if(0 != (ret = input_mt_create_slots(input_dev, 2))) {
#endif
		dev_info(zf->dev, "unable to get multitouch slot\n");
		goto out_slots;
	}

	if(0 != (ret = zforce_init(zf)))
		goto out_activate;

	//Optionally
	if(0 != (ret = zforce_init_ops(zf)))
		goto out_frequency;
	
	if(0 != (ret = zforce_get_status(zf, &zf->settings)))
		goto out_status;

	dev_info(zf->dev, "resolution %u by %u\n", zf->settings.res.width, zf->settings.res.height);
	dev_info(zf->dev, "finger_freq %u idle_freq %u stylus_freq %u \n", zf->settings.freq_data.finger_freq,
		zf->settings.freq_data.idle_freq, zf->settings.freq_data.stylus_freq);
	//dev_info(zf->dev, "scanning activated %x \n", zf->settings.active);
	//dev_info(zf->dev, "scanning counter %u \n", zf->settings.scan_counter);
	dev_info(zf->dev, "config flags %u \n", zf->settings.config.flags);
	//dev_info(zf->dev, "height %u width %u \n", zf->settings.phys.height, zf->settings.phys.width);
	//dev_info(zf->dev, "active led x: start %u end %u y: start %u end %u\n", zf->settings.leds.x_start,
		//zf->settings.leds.x_end,zf->settings.leds.y_start,zf->settings.leds.y_end); 
	dev_info(zf->dev, "max touch area enabled: %u size: %u \n", zf->settings.touch_size.max_enabled, 
		zf->settings.touch_size.max_size);
	//dev_info(zf->dev, "min touch area enabled: %u size: %u \n", zf->settings.touch_size.min_enabled, 
		//zf->settings.touch_size.min_size);

	/* These are all defined in Documentation/input/multi-touch-protocol.txt */
	input_set_abs_params(zf->input_dev, ABS_MT_POSITION_X, 0, zf->settings.res.width, 0, 0);
	input_set_abs_params(zf->input_dev, ABS_MT_POSITION_Y, 0, zf->settings.res.height, 0, 0);
	input_set_abs_params(zf->input_dev, ABS_MT_TOUCH_MAJOR, 0, 255, 0, 0);
	input_set_abs_params(zf->input_dev, ABS_MT_WIDTH_MAJOR, 0, 255, 0, 0);
	input_set_abs_params(zf->input_dev, ABS_MT_PRESSURE, 0, 255, 0, 0);
	input_set_abs_params(zf->input_dev, ABS_MT_TRACKING_ID, -1, 63, 0, 0);
	input_set_abs_params(zf->input_dev, ABS_MT_SLOT, 1, 2, 0, 0);
	input_set_abs_params(zf->input_dev, ABS_X, 0, zf->settings.res.width, 0, 0);
	input_set_abs_params(zf->input_dev, ABS_Y, 0, zf->settings.res.height, 0, 0);
	input_set_abs_params(zf->input_dev, ABS_PRESSURE, 0, zf->settings.res.width, 0, 0);
	/* 2*(slot, tracking id, x, y, width, touch) + sync */
	/* Where are these defined? */
	input_set_events_per_packet(zf->input_dev, 21); 
	
	dev_info(zf->dev, "Zforce version: %u.%u.%u.%u\n", zf->settings.fw_major, zf->settings.fw_minor, zf->settings.fw_build, zf->settings.fw_revision);

	zf->input_dev->id.product = zf->settings.fw_major;
	zf->input_dev->id.version = zf->settings.fw_revision;

	if(0 != (ret = input_register_device(input_dev)))
		goto out_register;

	/* Activate touch engine */
	if(0 != (ret = zforce_get_touchdata(zf)))
		goto out_touch;
		
	//Suspend & Resume trial
	//zforce_suspend(zf);
	
	//msleep(1);
	
	//if(0 != (ret = zforce_resume(zf)))
		//goto out_touch;
		
	/* Create a syfs interface to driver */	
	if(0 != (ret = zforce_create_sysfs(zf)))
		goto err_remove_group;
                                    

//	disable_irq(zf->irq);
	dev_info(zf->dev, "zforce_probe: zforce loaded \n");
	
	return 0;

err_remove_group:
	zforce_delete_sysfs(zf);
out_touch:
	input_unregister_device(input_dev);
	input_dev = NULL;
out_register:
out_status:
out_frequency:
out_activate:
	if(input_dev)
		input_mt_destroy_slots(input_dev);
out_slots:
	//dev_info(zf->dev, "zforce_probe: %d out slots\n", ret);
	free_irq(irq, zf);
out_bootfail:
	dev_info(zf->dev, "zforce_probe: Bootcomplete not received, Zforce failed to load \n");	
	destroy_workqueue(zf->zforce_tasks);
out_irq:
	gpio_free(zf->irq_line);
out_gpio:
	//dev_info(zf->dev, "zforce_probe: %d out_gpio\n", ret);
	gpio_free(NEONODE_RST);
	if(input_dev)
		input_free_device(input_dev);
out_alloc:
	dev_info(zf->dev, "Zforce failed to load, err:%d\n", ret);
	kfree(zf);
out:
	return ret;
}
EXPORT_SYMBOL(zforce_probe);

int zforce_remove(struct zforce *zf, unsigned int irq)
{
	int ret;
	dev_info(zf->dev, "zforce_remove: zforce removed\n");

	// Neonode reset has to be pulled low before it's deactivated
	ret = zforce_deactivate(zf);
	gpio_set_value_cansleep(NEONODE_RST, 0);

	input_mt_destroy_slots(zf->input_dev);
	free_irq(irq, zf);
	
	gpio_free(zf->irq_line);
	gpio_free(NEONODE_RST);

	switch(system_rev) {
		case LF2000_BOARD_RIO_BETA_1024_600:
		case LF2000_BOARD_RIO_BETA_800_480:
		case LF2000_BOARD_RIO_BETA_1024_600_700_400:
		case LF2000_BOARD_RIO_BETA_800_480_700_400:
		case LF2000_BOARD_RIO_BETA_1024_600_550_275:
		case LF2000_BOARD_RIO_EP_550_275:
		case LF2000_BOARD_RIO_EP_666_333:
		case LF2000_BOARD_RIO_EP_800_333:
		case LF2000_BOARD_RIO_EP_700_400:
		case LF2000_BOARD_RIO_EP_800_400:
		case LF2000_BOARD_RIO_FEP_800_327P67:
		case LF2000_BOARD_RIO_FEP_800_327P666:
			gpio_free(NEONODE_TEST);
			break;
		default:
			printk(KERN_ERR "%s: Invalid Board Revision 0x%X - Not beta board, no neonode test pin\n",
				__func__, system_rev);
			break;
	}

	//gpio_free(TP_DEBUG_NEONODE);
	
	destroy_workqueue(zf->zforce_tasks);
	
	zforce_delete_sysfs(zf);
	
	input_unregister_device(zf->input_dev);
	kfree(zf);
	
	return 0;
}
EXPORT_SYMBOL(zforce_remove);

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
	//struct zforce *zf;
	printk(KERN_INFO "zforce_i2c_probe \n");

	if(!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE))
	{
		dev_err(&client->dev, "Incompatible i2c host\n");
		return -EIO;
	}

	if(0 != (ret = zforce_probe(&client->dev, client->irq,
			  &zforce_i2c_bus_ops)))//, &zf)))
		return ret;

	i2c_set_clientdata(client, zfp);

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

MODULE_AUTHOR("Sukhada Palav <spalav@leapfrog.com>");
MODULE_DESCRIPTION("Neonode zForce touchscreen I2C bus driver");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("i2c:zforce");
