/*
 * Monitor runtime API
 *
 * Author      : Juan Encinas Anchústegui <juan.encinas@upm.es>
 * Date        : February 2021
 * Description : This file contains the Monitor runtime API, which can
 *               be used by any application to monitor the power consumption
 * 		 and performances of hardware accelerators.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <sys/mman.h>  // mmap()
#include <sys/ioctl.h> // ioctl()
#include <sys/poll.h>  // poll()
#include <sys/time.h>  // struct timeval, gettimeofday()

#include "drivers/monitor/monitor.h"
#include "monitor.h"
#include "monitor_hw.h"
#include "monitor_dbg.h"

#include <inttypes.h>


/*
 * Monitor global variables
 *
 * @monitor_hw : user-space map of Monitor hardware registers
 * @monitor_fd : /dev/monitor file descriptor (used to access kernels)
 *
 * @monitordata    : structure containing memory banks information
 *
 */
static int monitor_fd;
uint32_t *monitor_hw = NULL;
static struct monitorData_t *monitordata = NULL;

/*
 * Monitor init function
 *
 * This function sets up the basic software entities required to manage
 * the Monitor low-level functionality (DMA transfers, registers access, etc.).
 *
 * Return : 0 on success, error code otherwise
 */
int monitor_init() {
    const char *filename = "/dev/monitor";
    int ret;

    /*
     * NOTE: this function relies on predefined addresses for both control
     *       and data interfaces of the Monitor infrastructure.
     *       If the processor memory map is changed somehow, this has to
     *       be reflected in this file.
     *
     *       Zynq-7000 Devices
     *       Power memory bank  -> 0x20000000
     *       Traces memory bank -> 0x20040000
     *
     *       Zynq Ultrascale+ Devices
     *       Power memory bank  -> 0xb0100000
     *       Traces memory bank -> 0xb0180000
     *
     */

    // Open Monitor device file
    monitor_fd = open(filename, O_RDWR);
    if (monitor_fd < 0) {
        monitor_print_error("[monitor-hw] open() %s failed\n", filename);
        return -ENODEV;
    }
    monitor_print_debug("[monitor-hw] monitor_fd=%d | dev=%s\n", monitor_fd, filename);

    // Obtain access to physical memory map using mmap()
    monitor_hw = mmap(NULL, 0x10000, PROT_READ | PROT_WRITE, MAP_SHARED, monitor_fd, 0);
    if (monitor_hw == MAP_FAILED) {
        monitor_print_error("[monitor-hw] mmap() failed\n");
        ret = -ENOMEM;
        goto err_mmap;
    }
    monitor_print_debug("[monitor-hw] monitor_hw=%p\n", monitor_hw);

    // Initialize regions structure
    monitordata = malloc(sizeof *monitordata);
    if (!monitordata) {
        monitor_print_error("[monitor-hw] malloc() failed\n");
        ret = -ENOMEM;
        goto err_malloc_monitordata;
    }
    monitordata->power = NULL;
    monitordata->traces = NULL;
    monitor_print_debug("[monitor-hw] monitordata=%p\n", monitordata);

    return 0;

err_malloc_monitordata:
    munmap(monitor_hw, 0x10000);
err_mmap:
    close(monitor_fd);

    return ret;
}

/*
 * Monitor exit function
 *
 * This function cleans the software entities created by monitor_init().
 *
 */
void monitor_exit() {

    // Release allocated memory for monitordata
    free(monitordata);

    // Release memory obtained with mmap()
    munmap(monitor_hw, 0x10000);

    // Close ARTICo3 device file
    close(monitor_fd);
}

/*
 * Monitor normal voltage reference configuration function
 *
 * This function sets the monitor ADC voltage reference to 2.5V.
 *
 */
void monitor_config_vref(){

    monitor_hw_config_vref();

}

/*
 * Monitor double voltage reference configuration function
 *
 * This function sets the monitor ADC voltage reference to 5V.
 *
 */
void monitor_config_2vref(){

    monitor_hw_config_2vref();

}

/*
 * Monitor start function
 *
 * This function starts the monitor acquisition.
 *
 */
void monitor_start(){

    monitor_hw_start();

}

/*
 * Monitor clean function
 *
 * This function cleans the monitor memory banks.
 *
 */
void monitor_clean(){

    monitor_hw_clean();

}

/*
 * Monitor stop function
 *
 * This function stop the monitor acquisition. (only makes sense when power monitoring disabled)
 *
 */
void monitor_stop(){

    // Return if monitor is already done, otherwise stop it
    if (monitor_hw_isdone() == 1){
        return;
    }
    monitor_hw_stop();

}

/*
 * Monitor set mask function
 *
 * @mask : Triggering mask
 *
 * This function sets a mask used to decide which signals trigger the monitor execution.
 *
 */
void monitor_set_mask(int mask){

    monitor_hw_set_mask(mask);

}

/*
 * Monitor set AXI mask function
 *
 * @mask : AXI triggering mask
 *
 * This function sets a mask used to decide which AXI communication triggers the monitor execution.
 *
 */
void monitor_set_axi_mask(int mask){

    monitor_hw_set_axi_mask(mask);

}

/*
 * Monitor get acquisition time function
 *
 * This function gets the acquisition elapsed cycles used for data plotting in post-processing.
 *
 * Return : Elapsed cycles
 *
 */
int monitor_get_time(){

    return monitor_hw_get_time();

}

/*
 * Monitor get power measurements function
 *
 * This function gets the number of power consumption measurements stored in the BRAM used in post-processing.
 *
 * Return : Number of power consupmtion measurements
 *
 */
int monitor_get_number_power_measurements() {

    return monitor_hw_get_number_power_measurements();

}

/*
 * Monitor get traces measurements function
 *
 * This function gets the number of probes events stored in the BRAM used in post-processing.
 *
 * Return : Number of probes events
 *
 */
int monitor_get_number_traces_measurements() {

    return monitor_hw_get_number_traces_measurements();

}

/*
 * Monitor is done function
 *
 * This function checks if the acquisition has finished.
 *
 * Return : 1 if acquisition finished, 0 otherwise
 *
 */
int monitor_isdone(){

    return monitor_hw_isdone();

}

/*
 * Monitor is busy function
 *
 * This function checks if the monitor is busy.
 *
 * Return : 1 if busy, 0 if idle
 *
 */
int monitor_isbusy(){

    return monitor_hw_isbusy();

}

/*
 * Monitor get number of power errors function
 *
 * This function return the number of incorrect power samples received from ADC
 *
 * Return : Number of errors [0,$number_power_measurements]
 *
 */
int monitor_get_power_errors(){

    return monitor_hw_get_number_power_erros();

}


/*
 * Monitor no busy-wait waiting function
 *
 * This function waits for the monitor to finish in a not busy-wait manner.
 *
 */
void monitor_wait(){

    // Monitor management using interrupts and blocking system calls
	{ struct pollfd pfd = { .fd = monitor_fd, .events = POLLIRQ, };  poll(&pfd, 1, -1); }

}


/*
 * Monitor power consumption read function
 *
 * This function reads the monitor power consumption data sampled.
 *
 * @ndata   : amount of data to be read from power memory bank
 *
 * Return : 0 on success, error code otherwise
 *
 */
int monitor_read_power_consumption(unsigned int ndata) {
    struct dmaproxy_token token;
    monitorpdata_t *mem = NULL;

    struct pollfd pfd;
    pfd.fd = monitor_fd;
    pfd.events = POLLDMA;

    // Allocate DMA physical memory
    mem = mmap(NULL, ndata * sizeof *mem, PROT_READ | PROT_WRITE, MAP_SHARED, monitor_fd, sysconf(_SC_PAGESIZE));
    if (mem == MAP_FAILED) {
        monitor_print_error("[monitor-hw] mmap() failed\n");
        return -ENOMEM;
    }

    // Start DMA transfer
    token.memaddr = mem;
    token.memoff = 0x00000000;
    token.hwaddr = (void *)MONITOR_POWER_ADDR;
    token.hwoff = 0x00000000;
    token.size = ndata * sizeof *mem;
    ioctl(monitor_fd, MONITOR_IOC_DMA_HW2MEM_POWER, &token);

    // Wait for DMA transfer to finish
    poll(&pfd, 1, -1);


    if (!monitordata->power){
        monitor_print_error("[monitor-hw] no power region found (dma transfer)\n");
        return -1;
    }
    // Copy data from DMA-allocated memory buffer to userspace memory buffer

    memcpy(monitordata->power->data, mem, ndata * sizeof *mem);

    // Release allocated DMA memory
    munmap(mem, ndata * sizeof *mem);

    return 0;
}

/*
 * Monitor traces read function
 *
 * This function reads the monitor traces data sampled.
 *
 * @ndata   : amount of data to be read from traces memory bank
 *
 * Return : 0 on success, error code otherwise
 *
 */
int monitor_read_traces(unsigned int ndata) {
    struct dmaproxy_token token;
    monitortdata_t *mem = NULL;

    struct pollfd pfd;
    pfd.fd = monitor_fd;
    pfd.events = POLLDMA;

    // Allocate DMA physical memory
    mem = mmap(NULL, ndata * sizeof *mem, PROT_READ | PROT_WRITE, MAP_SHARED, monitor_fd, 2 * sysconf(_SC_PAGESIZE));
    if (mem == MAP_FAILED) {
        monitor_print_error("[monitor-hw] mmap() failed\n");
        return -ENOMEM;
    }

    // Start DMA transfer
    token.memaddr = mem;
    token.memoff = 0x00000000;
    token.hwaddr = (void *)MONITOR_TRACES_ADDR;
    token.hwoff = 0x00000000;
    token.size = ndata * sizeof *mem;
    ioctl(monitor_fd, MONITOR_IOC_DMA_HW2MEM_TRACES, &token);

    // Wait for DMA transfer to finish
    poll(&pfd, 1, -1);

    if (!monitordata->traces){
        monitor_print_error("[monitor-hw] no traces region found (dma transfer)\n");
        return -1;
    }

    // Copy data from DMA-allocated memory buffer to userspace memory buffer
    memcpy(monitordata->traces->data, mem, ndata * sizeof *mem);

    // Release allocated DMA memory
    munmap(mem, ndata * sizeof *mem);

    return 0;
}

/*
 * Monitor allocate buffer memory
 *
 * This function allocates dynamic memory to be used as a buffer between
 * the application and the local memories in the hardware kernels.
 *
 * @ndata   : amount of data to be allocated for the buffer
 * @regname : memory bank name to associate this buffer with
 * @regtype : memory bank type (power or traces)
 *
 * Return : pointer to allocated memory on success, NULL otherwise
 *
 */
void *monitor_alloc(int ndata, const char *regname, enum monitorregtype_t regtype) {
    struct monitorRegion_t *region = NULL;

    // Search for port in port lists
    if (monitordata->power){
        if (regtype == MONITOR_REG_POWER){
            monitor_print_error("[monitor-hw] power region already exist\n");
            return NULL;
        }
        if (strcmp(monitordata->power->name, regname) == 0){
            monitor_print_error("[monitor-hw] a region has been found with name %s\n", regname);
            return NULL;
        }
    }
    if (monitordata->traces){
        if (regtype == MONITOR_REG_TRACES){
            monitor_print_error("[monitor-hw] traces region already exist\n");
            return NULL;
        }
        if (strcmp(monitordata->traces->name, regname) == 0){
            monitor_print_error("[monitor-hw] a region has been found with name %s\n", regname);
            return NULL;
        }
    }

    // Allocate memory for kernel port configuration
    region = malloc(sizeof *region);
    if (!region) {
        return NULL;
    }

    // Set port name
    region->name = malloc(strlen(regname) + 1);
    if (!region->name) {
        goto err_malloc_reg_name;
    }
    strcpy(region->name, regname);

    // Set port size
    if (regtype == MONITOR_REG_POWER)
        region->size = ndata * sizeof(monitorpdata_t);

    if (regtype == MONITOR_REG_TRACES)
        region->size = ndata * sizeof(monitortdata_t);

    // Allocate memory for application
    region->data = malloc(region->size);
    if (!region->data) {
        goto err_malloc_region_data;
    }

    // Check port direction flag : POWER
    if (regtype == MONITOR_REG_POWER)
        monitordata->power = region;

    // Check port direction flag : TRACES
    if (regtype == MONITOR_REG_TRACES)
        monitordata->traces = region;

    // Return allocated memory
    return region->data;

err_malloc_reg_name:
    free(region->name);
    region->name = NULL;

err_malloc_region_data:
    free(region);
    region = NULL;

    return NULL;
}


/*
 * Monitor release buffer memory
 *
 * This function frees dynamic memory allocated as a buffer between the
 * application and the hardware kernel.
 *
 * @regname : memory bank this buffer is associated with
 *
 * Return : 0 on success, error code otherwise
 *
 */
int monitor_free(const char *regname) {
    struct monitorRegion_t *region = NULL;

    // Search for port in port lists
    if (monitordata->power != NULL){
        if (strcmp(monitordata->power->name, regname) == 0){
            region = monitordata->power;
            monitordata->power = NULL;
        }
    }
    if (monitordata->traces != NULL){
        if (strcmp(monitordata->traces->name, regname) == 0){
            region = monitordata->traces;
            monitordata->traces = NULL;
        }
    }

    if (region == NULL) {
        monitor_print_error("[monitor-hw] no region found with name %s\n", regname);
        return -ENODEV;
    }

    // Free application memory
    free(region->data);
    free(region->name);
    free(region);

    return 0;
}
