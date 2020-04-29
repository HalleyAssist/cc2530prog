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

#include "gpio.h"

#define SYSFS_GPIO	"/sys/class/gpio"

int value_fds[256];
int direction_fds[256];

void gpio_init()
{
	memset(value_fds, 0, sizeof(value_fds));
	memset(direction_fds, 0, sizeof(direction_fds));
}

int read_file(const char *path, char *str, size_t size)
{
	int fd;
	int ret;

	fd = open(path, O_RDONLY);
	if (fd < 0) {
		perror(path);
		return -1;
	}

	ret = read(fd, str, size - 1);
	if (ret < 0) {
		perror("read");
		close(fd);
		return -1;
	}

	close(fd);
	str[ret] = '\0';

	return 0;
}

int write_file(const char *path, const char *str)
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

static int open_fd_value(int n){
	char path[128];
	snprintf(path, sizeof (path), SYSFS_GPIO "/gpio%d/value", n);
	value_fds[n] = open(path, O_RDWR);
	if (value_fds[n] < 0) {
		perror(path);
		return -1;
	}
	return value_fds[n];
}
static int open_fd_direction(int n){
	char path[128];
	snprintf(path, sizeof (path), SYSFS_GPIO "/gpio%d/direction", n);
	direction_fds[n] = open(path, O_RDWR);
	if (direction_fds[n] < 0) {
		perror(path);
		return -1;
	}
	return direction_fds[n];
}

int
gpio_export(int n)
{
	char buf[16];

	snprintf(buf, sizeof (buf), "%d", n);

	return write_file(SYSFS_GPIO "/export", buf);
}

int gpio_unexport(int n)
{
	char buf[16];

	snprintf(buf, sizeof(buf), "%d", n);

	return write_file(SYSFS_GPIO "/unexport", buf);
}

int
gpio_set_direction(int n, enum gpio_direction direction)
{
	static const char *str[] = {
		[GPIO_DIRECTION_IN]	= "in",
		[GPIO_DIRECTION_OUT]	= "out",
		[GPIO_DIRECTION_HIGH]	= "high",
	};

	static const int lens[] = {
		[GPIO_DIRECTION_IN]	= 2,
		[GPIO_DIRECTION_OUT]	= 3,
		[GPIO_DIRECTION_HIGH]	= 4,
	};
	
	int ret;
	int fd = direction_fds[n];
	if(!fd){
		fd = open_fd_direction(n);
	}

	ret = write(fd, str[direction], lens[direction]);
	if (ret < 0) {
		if (errno == EBUSY)
			ret = 0;
		else
			perror("write");
	}else{
		ret = 0;
	}

	return ret;
}

int
gpio_get_value(int n, bool *value)
{
	int ret, fd;
	char buf[2];

	fd = value_fds[n];
	if(!fd){
		fd = open_fd_value(n);
	}
	
	ret = read(fd, buf, sizeof(buf));
	if(ret <= 0) {
		perror("read");
		return ret;
	}

	*value = (*buf != '0');

	return 0;
}

int
gpio_set_value(int n, bool value)
{
	int ret, fd;
	
	fd = value_fds[n];
	if(!fd){
		fd = open_fd_value(n);
	}
	
	ret = write(fd, value ? "1" : "0", 1);
	if (ret < 0) {
		if (errno == EBUSY)
			ret = 0;
		else
			perror("write");
	}else{
		ret = 0;
	}

	return ret;
}
