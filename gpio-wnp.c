/*
 * Linux GPIO backend using sysfs
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

#include <wiringPi.h>

void gpio_init()
{
	wiringPiSetupGpio();
}

int
gpio_export(int n)
{
	return 0;
}

int gpio_unexport(int n)
{
	return 0;
}

int
gpio_set_direction(int n, enum gpio_direction direction)
{
	static const int str[] = {
		[GPIO_DIRECTION_IN]	= OUTPUT,
		[GPIO_DIRECTION_OUT]	= INPUT
	};
	
	return pinMode (n, str[direction]);
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
	digitalWrite (n, (int)value);
	return 0;
}
