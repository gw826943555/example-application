/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>

#include <app/display.h>
#include <app/drivers/blink.h>

#include <app_version.h>

LOG_MODULE_REGISTER(main, CONFIG_APP_LOG_LEVEL);

#define BLINK_PERIOD_MS_STEP 100U
#define BLINK_PERIOD_MS_MAX  1000U

int main(void)
{
	int ret;
	const struct device *blink;

	printk("Zephyr Example Application %s\n", APP_VERSION_STRING);

	display_thread_start();

	blink = DEVICE_DT_GET(DT_NODELABEL(blink_led));
	if (!device_is_ready(blink)) {
		LOG_ERR("Blink LED not ready");
		return 0;
	}

	ret = blink_off(blink);
	if (ret < 0) {
		LOG_ERR("Could not turn off LED (%d)", ret);
		return 0;
	}

	printk("Use the sensor to change LED blinking period\n");

	while (1) {
		uint32_t period_ms = (uint32_t)(rand() % BLINK_PERIOD_MS_MAX);
		struct display_msg dmsg = {
			.kind = DISPLAY_MSG_POWER,
			.arg = period_ms,
		};

		blink_set_period_ms(blink, (int)period_ms);

		if (display_post(&dmsg, K_NO_WAIT) != 0) {
			printk("main: display queue full, drop update\n");
		}

		k_sleep(K_MSEC(1000));
	}

	return 0;
}

