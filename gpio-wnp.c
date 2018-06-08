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
#define SYSFS_GPIO "/sys/class/gpio"
#include <wiringPi.h>

static int write_count = 5;

// File writer from gpio-sysfs, see gpio_export comment
int
write_file(const char *path, const char *str)
{
	int fd;
	int ret;

	fd = open(path, O_WRONLY);
	if (fd < 0) {
		perror(path);
		return -1;
	}

	ret = write(fd, str, strlen(str));
	if (ret < 0) {
		if (errno == EBUSY)
			ret = 0;
		else
			perror("write");
	}

	close(fd);

	return ret < 0 ? -1 : 0;
}

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
	// WiringNP shouldn't require this, but testing shows that the RST_GPIO
	// needs to be exported with direction 'out' via sysfs. I don't know why.
	char buf[16];
	char path[128];
	snprintf(buf, sizeof (buf), "%d", n);
	snprintf(path, sizeof (path), SYSFS_GPIO "/gpio%d/direction", n);
	return write_file(SYSFS_GPIO "/export", buf) || write_file(path, "out");

}

int
gpio_unexport(int n)
{
	char buf[16];
	snprintf(buf, sizeof(buf), "%d", n);
	return write_file(SYSFS_GPIO "/unexport", buf);
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
