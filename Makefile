#
# Makefile for cc2530prog
#

CC?=gcc
CFLAGS?=
APP=cc2530prog
GPIO_BACKEND?=gpio-wnp

ifeq $(GPIO_BACKEND) 'gpio-wnp'
LIBS?=-lwiringPi
else
LIBS?=""
endif

all: $(APP)

%.o: %.c
	$(CC) $(CFLAGS) -DGPIO_BACKEND=$(GPIO_BACKEND) -c $< -o $@

OBJS=$(APP).o $(GPIO_BACKEND).o

$(APP): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $@

clean:
	rm -f *.o $(APP)
