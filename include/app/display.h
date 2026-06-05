/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef APP_DISPLAY_H_
#define APP_DISPLAY_H_

#include <zephyr/kernel.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Queue depth: main may burst a few updates before display catches up. */
#define DISPLAY_MSGQ_MAX_MSGS 8U

/** Message kinds sent from main (or other threads) to the display worker. */
enum display_msg_kind {
	/** arg: blink period in milliseconds */
	DISPLAY_MSG_POWER,
	DISPLAY_MSG_NORMAL,
	DISPLAY_MSG_BREAK,
	DISPLAY_MSG_TURN,
};

struct display_msg {
	uint32_t kind;
	uint32_t arg;
};

/**
 * Post one message to the display thread.
 *
 * @param msg Message to copy into the queue.
 * @param timeout How long to wait for a free slot (e.g. K_NO_WAIT, K_FOREVER).
 * @return 0 on success, negative errno otherwise (-EAGAIN if queue full with K_NO_WAIT).
 */
int display_post(const struct display_msg *msg, k_timeout_t timeout);

/** Spawn the display worker thread (idempotent). */
void display_thread_start(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_DISPLAY_H_ */
