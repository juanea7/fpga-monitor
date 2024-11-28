#
# MDC - Monitor Linux Application
#
# Author   : Juan Encinas <juan.encinas@upm.es>
# Date     : November 2024
#

CFLAGS_IN = -O3 
LDFLAGS_IN = -static 

CC = $(CROSS_COMPILE)gcc
MAKE = make

CFLAGS_MONITOR = $(CFLAGS_IN)-DZYNQMP -Wall -Wextra -fpic -I ../../../linux
LDFLAGS_MONITOR = -Wl,-R,. -shared
LDLIBS_MONITOR = -lm

CFLAGS_DMA = $(CFLAGS_IN)-Wall -Wextra -fpic
LDFLAGS_DMA = -Wl,-R,. -shared
LDLIBS_DMA = -lm

CFLAGS_APP = $(CFLAGS_IN)-DZYNQMP -Wall -Wextra -I monitor -I unica_dma -O3
LDFLAGS_APP = $(LDFLAGS_IN)-L . -Wl,-R,.
LDLIBS_APP = -lmonitor -ldma

# UPM+UNICA Monitor related parameters
OBJS1 = monitor/upm_monitor/monitor_hw.o monitor/upm_monitor/monitor.o monitor/unica_monitor/powermon.o monitor/unified_monitor.o
MONITOR_OBJS = $(OBJS1:%=_build/%)

# Unica DMA related parameters
OBJS2 = unica_dma/dma_simplemode.o
DMA_OBJS = $(OBJS2:%=_build/%)

OBJS3 = application/main.o
APP_OBJS = $(OBJS3:%=_build/%)

MKDIRP = mkdir -p
CPF = cp -f

.PHONY: app
app: monitor dma $(APP_OBJS)
	$(CC) $(LDFLAGS_APP) $(APP_OBJS) $(LDLIBS_APP) -o main

# Generate UPM + UNICA monitor lib
.PHONY: monitor
monitor: $(MONITOR_OBJS)
	$(CC) $(LDFLAGS_MONITOR) $^ $(LDLIBS_MONITOR) -o libmonitor.so
	$(AR) rcs libmonitor.a $^

# Generate UNICA dma lib
.PHONY: dma
dma: $(DMA_OBJS)
	$(CC) $(LDFLAGS_DMA) $^ $(LDLIBS_DMA) -o libdma.so
	$(AR) rcs libdma.a $^

.PHONY: clean
clean:
	rm -rf _build libmonitor.so libmonitor.a libdma.so libdma.a main

_build/monitor/%.o: monitor/%.c
	$(MKDIRP) $(@D)
	$(CC) $(CFLAGS_MONITOR) -c $< -o $@

_build/unica_dma/%.o: unica_dma/%.c
	$(MKDIRP) $(@D)
	$(CC) $(CFLAGS_DMA) -c $< -o $@

_build/application/%.o: application/%.c
	$(MKDIRP) $(@D)
	$(CC) $(CFLAGS_APP) -x c -c $< -o $@

_build/application/%.o: application/%.cpp
	$(MKDIRP) $(@D)
	$(CC) $(CFLAGS_APP) -x c -c $< -o $@
