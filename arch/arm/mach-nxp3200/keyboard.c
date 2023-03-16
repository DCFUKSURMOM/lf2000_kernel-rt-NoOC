
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/gpio_keys.h>

#include <linux/lf2000/gpio.h>	//For virtual GPIO mappings

#define COMMON_DEBOUNCE_INTERVAL 50

static struct gpio_keys_button lf2000_buttons[] = {
	{
		.type	= EV_KEY,
		.code	= KEY_A,
		.gpio	= BUTTON_A,
		.desc	= "A Button",
		.active_low	= 1,
		.debounce_interval = COMMON_DEBOUNCE_INTERVAL,
	},
	{
		.type	= EV_KEY,
		.code	= KEY_B,
		.gpio	= BUTTON_B,
		.desc	= "B Button",
		.active_low	= 1,
		.debounce_interval = COMMON_DEBOUNCE_INTERVAL,
	},
	{
		.type	= EV_KEY,
		.code	= KEY_UP,
		.gpio	= DPAD_UP,
		.desc	= "DPAD Up",
		.active_low	= 1,
		.debounce_interval = COMMON_DEBOUNCE_INTERVAL,
	},
	{
		.type	= EV_KEY,
		.code	= KEY_DOWN,
		.gpio	= DPAD_DOWN,
		.desc	= "DPAD Down",
		.active_low	= 1,
		.debounce_interval = COMMON_DEBOUNCE_INTERVAL,
	},
	{
		.type	= EV_KEY,
		.code	= KEY_LEFT,
		.gpio	= DPAD_LEFT,
		.desc	= "DPAD Left",
		.active_low	= 1,
		.debounce_interval = COMMON_DEBOUNCE_INTERVAL,
	},
	{
		.type	= EV_KEY,
		.code	= KEY_RIGHT,
		.gpio	= DPAD_RIGHT,
		.desc	= "DPAD Right",
		.active_low	= 1,
		.debounce_interval = COMMON_DEBOUNCE_INTERVAL,
	},
	{
		.type	= EV_KEY,
		.code	= KEY_RED,
		.gpio	= BUTTON_RED,
		.desc	= "Red Button",
		.active_low	= 1,
		.debounce_interval = COMMON_DEBOUNCE_INTERVAL,
	},
	{
		.type	= EV_KEY,
		.code	= KEY_U,
		.gpio	= BUTTON_VOLUMEUP,
		.desc	= "Volume Up",
		.active_low	= 1,
		.debounce_interval = COMMON_DEBOUNCE_INTERVAL,
	},
	{
		.type	= EV_KEY,
		.code	= KEY_D,
		.gpio	= BUTTON_VOLUMEDOWN,
		.desc	= "Volume Down",
		.active_low	= 1,
		.debounce_interval = COMMON_DEBOUNCE_INTERVAL,
	},
	{
		.type	= EV_KEY,
		.code	= KEY_L,
		.gpio	= SHOULDER_LEFT,
		.desc	= "Left Shoulder",
		.active_low	= 1,
		.debounce_interval = COMMON_DEBOUNCE_INTERVAL,
	},
	{
		.type	= EV_KEY,
		.code	= KEY_R,
		.gpio	= SHOULDER_RIGHT,
		.desc	= "Right Shoulder",
		.active_low	= 1,
		.debounce_interval = COMMON_DEBOUNCE_INTERVAL,
	},
	{
		.type	= EV_KEY,
		.code	= KEY_ENTER,
		.gpio	= BUTTON_HOME,
		.desc	= "Home Button",
		.active_low	= 1,
		.debounce_interval = COMMON_DEBOUNCE_INTERVAL,
	},
	{
		.type	= EV_KEY,
		.code	= KEY_H,
		.gpio	= BUTTON_HINT,
		.desc	= "Hint Button",
		.active_low	= 1,
		.debounce_interval = COMMON_DEBOUNCE_INTERVAL,
	},
	{
		.type	= EV_KEY,
		.code	= KEY_P,
		.gpio	= BUTTON_PAUSE,
		.desc	= "Pause Button",
		.active_low	= 1,
		.debounce_interval = COMMON_DEBOUNCE_INTERVAL,
	},
	{
		.type	= EV_KEY,
		.code	= KEY_X,
		.gpio	= BUTTON_BRIGHTNESS,
		.desc	= "Brightness Button",
		.active_low	= 1,
		.debounce_interval = COMMON_DEBOUNCE_INTERVAL,
	},
	{
		.type	= EV_KEY,
		.code	= KEY_ESC,
		.gpio	= BUTTON_ESC,
		.desc	= "Escape Button",
		.active_low	= 1,
		.debounce_interval = COMMON_DEBOUNCE_INTERVAL,
	},
	{
		.type	= EV_SW,
		.code	= SW_HEADPHONE_INSERT,
		.gpio	= HEADPHONE_JACK,
		.desc	= "Headphone Jack",
		.active_low	= 0,
		.debounce_interval = COMMON_DEBOUNCE_INTERVAL,
	},
};

static struct gpio_keys_platform_data lf2000_button_data = {
	.buttons	= lf2000_buttons,
	.nbuttons	= ARRAY_SIZE(lf2000_buttons),
	.rep		= false,
};

struct platform_device lf2000_gpio_buttons = {
	.name		= "gpio-keys",
	.id		= -1,
	.dev		= {
		.platform_data	= &lf2000_button_data,
	},
};
