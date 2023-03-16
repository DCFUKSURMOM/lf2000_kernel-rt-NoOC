/*
 * ZForce touch screen driver smaple code for the I2C bus.
 *
 * Copyright (c) 2011 Neonode Technologies AB
 *
 * This source code is provided as a sample only and has not went though internal testing. This 
 * sample code is provided "as is", with no  warranties for the functionality of the code, nor 
 * any warranties concerning side effects when using the code. The intention of this code is 
 * reference example for internal evaluation only.
 * 
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/i2c.h>
#include <linux/timer.h>

// The command id's used by the ZForce

#define ZF_DEACTIVATE 0x00
#define ZF_ACTIVATE 0x01
#define ZF_SET_RESOLUTION 0x02
#define ZF_CONFIGURE 0x03
#define ZF_REQ_COORDINATES 0x04
#define ZF_REQ_VERSION 0x0A

// Bit flags for the startup state machinery. Response flags are set at response from the
// ZForce, but ignored by the program logic

#define ZF_VERSION_REQUESTED 0x0001
#define ZF_VERSION_RESPONDED 0x0002
#define ZF_DEACTIVATE_REQUESTED 0x0004
#define ZF_DEACTIVATE_RESPONDED 0x0008
#define ZF_ACTIVATE_REQUESTED 0x0010
#define ZF_ACTIVATE_RESPONDED 0x0020
#define ZF_RESOLUTION_REQUESTED 0x0040
#define ZF_RESOLUTION_RESPONDED 0x0080
#define ZF_CONFIGURE_REQUESTED 0x0100
#define ZF_CONFIGURE_RESPONDED 0x0200
#define ZF_FIRSTTOUCH_REQUESTED 0x0400
#define ZF_FIRSTTOUCH_RESPONDED 0x0800

#define ZF_MAX_X 10000
#define ZF_MAX_Y 10000

// Per- ZForce-device data
struct zforce_ts_priv {
	struct i2c_client *client;  // I2C client to communicate with the ZF chip
	struct input_dev *input;    // The registered input device to which events are reported
	struct delayed_work work;   // Scheduler entry for ISR "bottom half"
	int irq;   // IRQ on which ZF signals data available
	int startup_state; // Bit flags according to ZF_xx_REQUESTED/RESPONDED definitions
};

static void zforce_ts_handle_deactivate(u_int8_t *data, int len);
static void zforce_ts_handle_activate(u_int8_t *data, int len);
static void zforce_ts_handle_configure(u_int8_t *data, int len);
static void zforce_ts_handle_setres(u_int8_t *data, int len);
static void zforce_ts_handle_touchdata(struct zforce_ts_priv *priv,
		u_int8_t *data, int len);
static void zforce_ts_handle_version(u_int8_t *data, int len);
static int zforce_ts_send_deactivate(struct zforce_ts_priv *priv);
static int zforce_ts_send_activate(struct zforce_ts_priv *priv);
static int
zforce_ts_send_configure(struct zforce_ts_priv *priv, int dual_touch);
static int zforce_ts_send_setres(struct zforce_ts_priv *priv, int width,
		int height);
static int zforce_ts_send_touchdata_req(struct zforce_ts_priv *priv);
static int zforce_ts_send_version_req(struct zforce_ts_priv *priv);

static void zforce_ts_handle_data(struct work_struct *work) {

	// The container struct of the work_struct passed to the schedule call happens to be
	// the private data associated with this driver, use the container_of
	// macro to acquire it
	struct zforce_ts_priv *priv = container_of(work, struct zforce_ts_priv, work.work);

	// Current command received
	int zforce_command;

	// Length of payload part of received block
	int payload_length;

	// Data buffer (large enough to hold biggest expected block)
	u_int8_t tmp_buf[20];

	int more_verbose;

	int i;

	memset(tmp_buf, 0, sizeof(tmp_buf));

	// Be more verbose until one touch coordinate was handled
	more_verbose = priv->startup_state & ZF_FIRSTTOUCH_RESPONDED ? 0 : 1;

	// Read the first three bytes to get the command id and the size of the rest of the message
	if (i2c_master_recv(priv->client, tmp_buf, 3) != 3) {
		dev_err(&priv->client->dev, "Unable to read ZForce data header\n");
		goto out;
	}

	// Check the start byte
	if (tmp_buf[0] != 0xee) {

		dev_err(&priv->client->dev,
				"Invalid initial byte of ZForce data block\n");
		goto out;
	}

	// Get the command
	zforce_command = tmp_buf[1];

	// Get the length of the payload
	payload_length = tmp_buf[2];

	if (more_verbose) {

		printk("Data from ZForce, command=%d, length=%d\n", zforce_command,
				payload_length);
	}

	// The block is too long to handle
	if (payload_length > sizeof(tmp_buf)) {

		dev_err(&priv->client->dev, "Block from Zforce was too long\n");

		// Read byte by byte to flush the buffer

		for (i = 0; i < payload_length; i++) {

			i2c_master_recv(priv->client, tmp_buf, 1);
		}

		return;
	}

	// Reuse the data buffer and read the payload part of the block
	if (i2c_master_recv(priv->client, tmp_buf, payload_length)
			!= payload_length) {
		dev_err(&priv->client->dev, "Unable to get ZForce data header\n");
		goto out;
	}

	switch (zforce_command) { // Attend to the command

	case ZF_DEACTIVATE: // Got response from deactivate request

		priv->startup_state |= ZF_DEACTIVATE_RESPONDED;
		zforce_ts_handle_deactivate(tmp_buf, payload_length);

		break;

	case ZF_ACTIVATE: // Got response from activate request

		priv->startup_state |= ZF_ACTIVATE_RESPONDED;
		zforce_ts_handle_activate(tmp_buf, payload_length);

		break;

	case ZF_CONFIGURE: // Got response from configuration request

		priv->startup_state |= ZF_CONFIGURE_RESPONDED;
		zforce_ts_handle_configure(tmp_buf, payload_length);

		break;

	case ZF_SET_RESOLUTION: // Got response from resolution setting request

		priv->startup_state |= ZF_RESOLUTION_RESPONDED;
		zforce_ts_handle_setres(tmp_buf, payload_length);

		break;

	case ZF_REQ_COORDINATES: // Got touch event

		zforce_ts_handle_touchdata(priv, tmp_buf, payload_length);
		priv->startup_state |= ZF_FIRSTTOUCH_RESPONDED;

		break;

	case ZF_REQ_VERSION: // Got version

		priv->startup_state |= ZF_VERSION_RESPONDED;
		zforce_ts_handle_version(tmp_buf, payload_length);

		break;
	}

	if (!(priv->startup_state & ZF_DEACTIVATE_REQUESTED)) {

		// Still not deactivated, send a deactivation request, this is
		// necessary as the ZForce will potentially stop sending coordinates
		// if an activation is issued without prior deactivation

		priv->startup_state |= ZF_DEACTIVATE_REQUESTED;
		zforce_ts_send_deactivate(priv);
	} else if (!(priv->startup_state & ZF_ACTIVATE_REQUESTED)) {

		// Still not activated, send an activation request

		priv->startup_state |= ZF_ACTIVATE_REQUESTED;
		zforce_ts_send_activate(priv);
	} else if (!(priv->startup_state & ZF_RESOLUTION_REQUESTED)) {

		// Still no resolution set, send a resolution setting request

		priv->startup_state |= ZF_RESOLUTION_REQUESTED;
		zforce_ts_send_setres(priv, ZF_MAX_X, ZF_MAX_Y);
	} else if (!(priv->startup_state & ZF_CONFIGURE_REQUESTED)) {

		// Still not configured, send a configuration request

		priv->startup_state |= ZF_CONFIGURE_REQUESTED;
		zforce_ts_send_configure(priv, 1);
	} else {

		// All setup done, request some coordinates

		priv->startup_state |= ZF_FIRSTTOUCH_REQUESTED;
		zforce_ts_send_touchdata_req(priv);
	}

	out:

	// Re-enable IRQ so that we can handle the next ZForce event
	enable_irq(priv->irq);
}

static void zforce_ts_handle_deactivate(u_int8_t *data, int len) {

	printk("ZForce deactivation response, status=%d", data[0]);
}

static void zforce_ts_handle_activate(u_int8_t *data, int len) {

	printk("ZForce activation response, status=%d", data[0]);
}

static void zforce_ts_handle_configure(u_int8_t *data, int len) {

	printk("ZForce configuration response, status=%d", data[0]);
}

static void zforce_ts_handle_setres(u_int8_t *data, int len) {

	printk("ZForce resolution setting response, status=%d", data[0]);
}

// Handle touch coordinate events form the ZF
static void zforce_ts_handle_touchdata(struct zforce_ts_priv *priv,
		u_int8_t *payload, int len) {

	// Be more verbose for the very first touch coordinate we get
	int more_verbose = priv->startup_state & ZF_FIRSTTOUCH_RESPONDED ? 0 : 1;

	// The number of touch coordinates reported by the ZForce
	int coordinate_count = payload[0];

	// Used to automatically determine protocol version
	int protocol_variant = 0;

	// Index within the coordinate sub-data blocks
	int coord_index;

	int x;
	int y;
	int status;

	int i;

	// This is a QaD trick to determine which of the two protocol flavors we got (one uses 5 bytes
	//  per coordinate, the other uses 7 bytes per coordinate). The "protocol_variant" is set to the
	//  coordinate block length

	if (len == coordinate_count * 5 + 1) {

		if (more_verbose) {

			printk("Using 5 bytes per coordinate protocol\n");
		}

		// Older ZForce with 5 bytes per coordinate
		protocol_variant = 5;
	} else if (len == coordinate_count * 7 + 1) {

		if (more_verbose) {

			printk("Using 5 bytes per coordinate protocol\n");
		}

		// Newer ZForce with 7 bytes per coordinate
		protocol_variant = 7;
	}

	// Did we figure out the protocol ?
	if (!protocol_variant) {

		// Not the expected packet length

		dev_err(&priv->client->dev,
				"Could not match ZForce block length to any protocol\n");

		return;
	}

	for (i = 1; i < len; i++) { // Scan the buffer (skip the coordinate count)

		int data = payload[i];

		// The index within the multi-touch coordinate we are currently parsing
		coord_index = (i - 1) % protocol_variant;

		if (more_verbose && !coord_index) {

			printk("Parsing ZForce coordinate %d\n", i / protocol_variant);
		}

		switch (coord_index) {
		case 0: // X LSB

			x = data;
			break;

		case 1: // X MSB

			x |= (data << 8);
			break;

		case 2: // Y LSB

			y = data;
			break;

		case 3: // Y MSB

			y |= (data << 8);
			break;

		case 4: // Status

			status = data;

			if (more_verbose) {

				printk("Status=0x0%x x=%d y=%d\n", status, x, y);
			}

			// The status bits differ between the two "protocol variants"
			switch (protocol_variant == 7 ? status >> 6 : status & 3) {

			case 0: // Touch down
				input_report_abs(priv->input, ABS_X, x);
				input_report_abs(priv->input, ABS_Y, y);
				input_report_key(priv->input, BTN_TOUCH, 1);
				break;

			case 1: // Move
				input_report_abs(priv->input, ABS_X, x);
				input_report_abs(priv->input, ABS_Y, y);
				break;

			case 2: // Up
				input_report_key(priv->input, BTN_TOUCH, 0);
				break;

			}

			break;

		case 5: // Reserved byte
			break;

		case 6: // Probability, disregarded for the moment
			break;
		}

		if (i % protocol_variant) {

			// This is the last byte of the coordinate, so we can fire off the MT sync here
			input_mt_sync(priv->input);

			if (more_verbose) {

				printk("Syncing touch coordinate\n");
			}
		}

	}

	// Final sync
	input_sync(priv->input);

	if (more_verbose) {

		printk("Syncing multi touch event\n");
	}
}

static void zforce_ts_handle_version(u_int8_t *data, int len) {

	u_int16_t major = data[0] + (data[1] << 8);
	u_int16_t minor = data[2] + (data[3] << 8);
	u_int16_t build = data[4] + (data[5] << 8);
	u_int16_t revision = data[6] + (data[7] << 8);

	printk("ZForce version %d.%d, build %d, rev %d\n", major, minor, build,
			revision);
}

static int zforce_ts_send_deactivate(struct zforce_ts_priv *priv) {

	u_int8_t data[] = { ZF_DEACTIVATE };

	printk("Deactivating zForce\n");

	return i2c_master_send(priv->client, data, sizeof(data));
}

static int zforce_ts_send_activate(struct zforce_ts_priv *priv) {

	u_int8_t data[] = { ZF_ACTIVATE };

	printk("Activating zForce\n");

	return i2c_master_send(priv->client, data, sizeof(data));
}

static int zforce_ts_send_configure(struct zforce_ts_priv *priv, int dual_touch) {

	u_int8_t data[] = { ZF_CONFIGURE, 0, 0, 0, 0 };

	printk("Configuring zForce, using %s touch\n",
			dual_touch ? "dual" : "single");

	data[1] = dual_touch ? 0x01 : 0x00;

	return i2c_master_send(priv->client, data, sizeof(data));
}

static int zforce_ts_send_setres(struct zforce_ts_priv *priv, int width,
		int height) {

	u_int8_t data[] = { ZF_SET_RESOLUTION, 0, 0, 0, 0 };

	printk("Setting ZForce resolution, width=%d, height=%d\n", width, height);

	data[1] = width & 0xff;
	data[2] = width >> 8;
	data[3] = height & 0xff;
	data[4] = height >> 8;

	return i2c_master_send(priv->client, data, sizeof(data));
}

static int zforce_ts_send_touchdata_req(struct zforce_ts_priv *priv) {

	u_int8_t data[] = { ZF_REQ_COORDINATES };

	if (!(priv->startup_state & ZF_FIRSTTOUCH_RESPONDED)) {

		printk("Requesting touch coordinates\n");
	}

	return i2c_master_send(priv->client, data, sizeof(data));
}

static int zforce_ts_send_version_req(struct zforce_ts_priv *priv) {

	u_int8_t data[] = { ZF_REQ_VERSION };

	printk("Getting ZForce version\n");

	return i2c_master_send(priv->client, data, sizeof(data));
}

static irqreturn_t zforce_ts_isr(int irq, void *dev_id) {
	struct zforce_ts_priv *priv = dev_id;

	disable_irq_nosync(irq);

	schedule_delayed_work(&priv->work, HZ / 20);

	return IRQ_HANDLED;
}

static int zforce_ts_open(struct input_dev *dev) {
	struct zforce_ts_priv *priv = input_get_drvdata(dev);
	struct i2c_client *client = priv->client;

	// Fire off a version request to get the state machinery up and running, the rest
	// is done by the ISR handling routine
	if (zforce_ts_send_version_req(priv) < 0) {

		dev_err(&client->dev,
				"Unable to request version from ZForce touchscreen.\n");
		return -ENXIO;
	}

	return 0;
}

static void zforce_ts_close(struct input_dev *dev) {
	struct zforce_ts_priv *priv = input_get_drvdata(dev);

	disable_irq(priv->irq);

	// Cancel any pending bottom half work (wait for it to finish)
	if (cancel_delayed_work_sync(&priv->work)) {

		// Re-enable IRQ as it might have been disabled by the "top half" ISR
		enable_irq(priv->irq);
	}

	//TOOD: Maybe this should be done before cancelling work...
	// Deactivate the touch screen
	zforce_ts_send_deactivate(priv);

	enable_irq(priv->irq);
}

static int zforce_ts_probe(struct i2c_client *client,
		const struct i2c_device_id *idp) {

	struct zforce_ts_priv *priv;
	struct input_dev *input;
	int error;

	priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	if (!priv) {
		dev_err(&client->dev, "Failed to allocate driver data\n");
		error = -ENOMEM;
		goto err0;
	}

	dev_set_drvdata(&client->dev, priv);

	input = input_allocate_device();
	if (!input) {
		dev_err(&client->dev, "Failed to allocate input device.\n");
		error = -ENOMEM;
		goto err1;
	}

	input->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
	input->keybit[BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH);

	// Setup the coordinate system (span from 0 to MAX in both axis)
	input_set_abs_params(input, ABS_X, 0, ZF_MAX_X, 0, 0);
	input_set_abs_params(input, ABS_Y, 0, ZF_MAX_Y, 0, 0);

	input->name = client->name;
	input->id.bustype = BUS_I2C;
	input->dev.parent = &client->dev;

	input->open = zforce_ts_open;
	input->close = zforce_ts_close;

	input_set_drvdata(input, priv);

	priv->client = client;
	priv->input = input;

	INIT_DELAYED_WORK(&priv->work, zforce_ts_handle_data);

	priv->irq = client->irq;

	error = input_register_device(input);
	if (error)
		goto err1;

	//TODO: Even though the ISR is setup here, the IRQ line must be setup in the init
	//  or in the board support routine. For OMAP, the OMAP_GPIO_IRQ can be used to convert
	//  pin number into IRQ
	// See gpio_request, gpio_direction_input, gpio_export
	// See http://old.nabble.com/GPIO-programming-and-Interrupts-td24324179.html

	// See drivers/input/touchscreen/mainstone-wm97xx.c


	error = request_irq(priv->irq, zforce_ts_isr, IRQF_TRIGGER_LOW,
			client->name, priv);
	if (error) {
		dev_err(&client->dev, "Unable to request touchscreen IRQ.\n");
		goto err2;
	}

	device_init_wakeup(&client->dev, 1);
	return 0;

	err2: input_unregister_device(input);
	input = NULL; /* so we dont try to free it below */
	err1: input_free_device(input);
	kfree(priv);
	err0: dev_set_drvdata(&client->dev, NULL);
	return error;
}

static int zforce_ts_remove(struct i2c_client *client) {
	struct zforce_ts_priv *priv = dev_get_drvdata(&client->dev);

	free_irq(priv->irq, priv);
	input_unregister_device(priv->input);
	kfree(priv);

	dev_set_drvdata(&client->dev, NULL);

	return 0;
}

static int zforce_ts_suspend(struct i2c_client *client, pm_message_t mesg) {
	struct zforce_ts_priv *priv = dev_get_drvdata(&client->dev);

	if (device_may_wakeup(&client->dev))
		enable_irq_wake(priv->irq);

	return 0;
}

static int zforce_ts_resume(struct i2c_client *client) {
	struct zforce_ts_priv *priv = dev_get_drvdata(&client->dev);

	if (device_may_wakeup(&client->dev))
		disable_irq_wake(priv->irq);

	return 0;
}

static const struct i2c_device_id zforce_ts_id[] = { { "zforce", 0 }, { } };
MODULE_DEVICE_TABLE(i2c, zforce_ts_id);

static struct i2c_driver zforce_ts_driver = { .driver =
		{ .name = "zforcei2c", }, .probe = zforce_ts_probe, .remove =
		zforce_ts_remove, .suspend = zforce_ts_suspend, .resume =
		zforce_ts_resume, .id_table = zforce_ts_id, };



MODULE_DESCRIPTION("Neonode ZForce touchscreen driver for I2C bus");

module_init(zforce_ts_init);
module_exit(zforce_ts_exit);

