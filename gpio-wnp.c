/*
 * Linux GPIO backend using WiringNP
 *
 * Copyright (C) 2010, Florian Fainelli <f.fainelli@gmail.com>
 *
 * This file is part of "cc2530prog", this file is distributed under
 * a 2-clause BSD license, see LICENSE for details.
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include "gpio.h"

#include <wiringPi.h>

static int write_count = 5;

void
gpio_init()
{
	wiringPiSetupGpio();
	// Bits are banged multiple (WNP_WRITE_COUNT||5) times. This is probably
	// equivalent to a busy loop but is more tunable than counting to 10^n.
	const char* s = getenv("WNP_WRITE_COUNT");
	if (s == NULL) return;
	int env_write_count = atoi(s);
	if (env_write_count > 0) {
		write_count = env_write_count;
	}
}

int
gpio_export(int n)
{
	return 0;
}

int
gpio_unexport(int n)
{
	return 0;
}

int
gpio_set_direction(int n, enum gpio_direction direction)
{
	static const int str[] = {
		[GPIO_DIRECTION_IN]  = INPUT,
		[GPIO_DIRECTION_OUT] = OUTPUT
	};
	pinMode(n, str[direction]);
	return 0;
}

int
gpio_get_value(int n, bool *value)
{
	int res = digitalRead(n);
	*value = res != 0;
	return 0;
}

int
gpio_set_value(int n, bool value)
{
	for (int i = 0; i < write_count; i++) {
		digitalWrite(n, (int)value);
	}
	return 0;
}
