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

CFLAGS_MONITOR = $(CFLAGS_IN) -Wall -Wextra -fpic -I ../../../linux
LDFLAGS_MONITOR = -Wl,-R,. -shared
LDLIBS_MONITOR = -lm

CFLAGS_AUX = $(CFLAGS_IN)-Wall -Wextra -fpic
LDFLAGS_AUX = -Wl,-R,. -shared
LDLIBS_AUX = -lm

CFLAGS_APP = $(CFLAGS_IN)-DZYNQMP -Wall -Wextra -I monitor -I unica_dma -I cpu_usage -O3
LDFLAGS_APP = $(LDFLAGS_IN)-L . -Wl,-R,.
LDLIBS_APP = -lmonitor -ldma -lcpuusage -lpthread

# UPM+UNICA Monitor related parameters
OBJS1 = monitor/upm_monitor/monitor_hw.o monitor/upm_monitor/monitor.o monitor/unica_monitor/powermon.o monitor/unified_monitor.o
MONITOR_OBJS = $(OBJS1:%=_build/%)

# Unica DMA related parameters
OBJS2 = unica_dma/dma_simplemode.o
DMA_OBJS = $(OBJS2:%=_build/%)

# CPU usage
OBJS3 = cpu_usage/cpu_usage.o
CPU_USAGE_OBJS = $(OBJS3:%=_build/%)

OBJS4 = application/main.o
APP_OBJS = $(OBJS4:%=_build/%)

MKDIRP = mkdir -p
CPF = cp -f

.PHONY: app
app: monitor dma cpu_usage $(APP_OBJS)
	$(CC) $(LDFLAGS_APP) $(APP_OBJS) $(LDLIBS_APP) -o main

# Generate UPM + UNICA monitor lib
.PHONY: monitor
monitor: $(MONITOR_OBJS)
	$(CC) $(LDFLAGS_MONITOR) $^ $(LDLIBS_MONITOR) -o libmonitor.so
	$(AR) rcs libmonitor.a $^

# Generate UNICA dma lib
.PHONY: dma
dma: $(DMA_OBJS)
	$(CC) $(LDFLAGS_AUX) $^ $(LDLIBS_AUX) -o libdma.so
	$(AR) rcs libdma.a $^

# Generate cpu_usage lib
.PHONY: cpu_usage
cpu_usage: $(CPU_USAGE_OBJS)
	$(CC) $(LDFLAGS_AUX) $^ $(LDLIBS_AUX) -o libcpuusage.so
	$(AR) rcs libcpuusage.a $^

.PHONY: clean
clean:
	rm -rf _build libmonitor.so libmonitor.a libdma.so libdma.a libcpuusage.so libcpuusage.a main

_build/monitor/%.o: monitor/%.c
	$(MKDIRP) $(@D)
	$(CC) $(CFLAGS_MONITOR) -c $< -o $@

_build/unica_dma/%.o: unica_dma/%.c
	$(MKDIRP) $(@D)
	$(CC) $(CFLAGS_AUX) -c $< -o $@

_build/cpu_usage/%.o: cpu_usage/%.c
	$(MKDIRP) $(@D)
	$(CC) $(CFLAGS_AUX) -c $< -o $@

_build/application/%.o: application/%.c
	$(MKDIRP) $(@D)
	$(CC) $(CFLAGS_APP) -x c -c $< -o $@

_build/application/%.o: application/%.cpp
	$(MKDIRP) $(@D)
	$(CC) $(CFLAGS_APP) -x c -c $< -o $@
