/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 *
 * Display worker: lowest preemptible application priority (still above idle).
 * Communicates with main via k_msgq.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/atomic.h>

#include <app/display.h>

#define DISPLAY_STACK_SIZE 512U

K_THREAD_STACK_DEFINE(display_stack, DISPLAY_STACK_SIZE);

K_MSGQ_DEFINE(display_msgq, sizeof(struct display_msg), DISPLAY_MSGQ_MAX_MSGS, 4);

static struct k_thread display_thread_data;
static atomic_t display_started;

int display_post(const struct display_msg *msg, k_timeout_t timeout)
{
	return k_msgq_put(&display_msgq, msg, timeout);
}

static int display_power(int power)
{
	return 1000;
}

static void display_thread(void *p1, void *p2, void *p3)
{
	struct display_msg msg;

	ARG_UNUSED(p1);
	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

    int delay = 0;
	for (;;) {
		/* Wait for work; periodic timeout allows future "idle refresh" if needed */
		if (k_msgq_get(&display_msgq, &msg, K_MSEC(delay)) != 0) {
			continue;
		}

		switch (msg.kind) {
		case DISPLAY_MSG_POWER:
			printk("display: power %d\n", msg.arg);
            delay = display_power(msg.arg);
			break;
		case DISPLAY_MSG_NORMAL:
			printk("display: normal\n");
			break;
		case DISPLAY_MSG_BREAK:
			printk("display: break\n");
			break;
		case DISPLAY_MSG_TURN:
			printk("display: turn\n");
			break;
		default:
			printk("display: unknown kind %u\n", msg.kind);
			break;
		}
	}
}

void display_thread_start(void)
{
	if (!atomic_cas(&display_started, 0, 1)) {
		return;
	}

	k_thread_create(&display_thread_data, display_stack,
			K_THREAD_STACK_SIZEOF(display_stack), display_thread, NULL, NULL, NULL,
			K_LOWEST_APPLICATION_THREAD_PRIO, 0, K_NO_WAIT);
	k_thread_name_set(&display_thread_data, "display");
}
