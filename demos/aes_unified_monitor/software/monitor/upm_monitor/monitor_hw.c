/*
 * Monitor low-level hardware API
 *
 * Author      : Juan Encinas Anchústegui <juan.encinas@upm.es>
 * Date        : February 2021
 * Description : This file contains the low-level functions required to
 *               work with the Monitor infrastructure (Monitor registers).
 *
 */


#include <stdint.h>
#include <sys/types.h>
#include <errno.h>

#include "monitor_hw.h"
#include "monitor_dbg.h"

/*
 * Monitor normal voltage reference configuration function
 *
 * This function sets the monitor ADC voltage reference to 2.5V.
 *
 */
void monitor_hw_config_vref() {

    monitor_hw[MONITOR_REG0] = MONITOR_CONFIG_VREF;
    monitor_print_debug("[monitor-hw] set ADC reference voltage to 2.5V\n");

}

/*
 * Monitor double voltage reference configuration function
 *
 * This function sets the monitor ADC voltage reference to 5V.
 *
 */
void monitor_hw_config_2vref() {

    monitor_hw[MONITOR_REG0] = MONITOR_CONFIG_2VREF;
    monitor_print_debug("[monitor-hw] set ADC reference voltage to 5V\n");

}

/*
 * Monitor start function
 *
 * This function starts the monitor acquisition.
 *
 */
void monitor_hw_start() {

    while((monitor_hw[MONITOR_REG0] & MONITOR_BUSY) > 0);
    monitor_hw[MONITOR_REG0] = MONITOR_START;
    monitor_print_debug("[monitor-hw] start to monitor power consumption and traces\n");

}

/*
 * Monitor clean function
 *
 * This function cleans the monitor memory banks.
 *
 */
void monitor_hw_clean() {

    monitor_hw[MONITOR_REG0] = MONITOR_STOP;
    monitor_print_debug("[monitor-hw] clean brams\n");

}

/*
 * Monitor stop function
 *
 * This function stop the monitor acquisition. (only makes sense when power monitoring disabled)
 *
 */
void monitor_hw_stop() {

    monitor_hw[MONITOR_REG0] = MONITOR_STOP;
    monitor_print_debug("[monitor-hw] stop acquisition\n");

}

/*
 * Monitor set mask function
 *
 * @mask : Triggering mask
 *
 * This function sets a mask used to decide which signals trigger the monitor execution.
 *
 */
void monitor_hw_set_mask(int mask) {

    monitor_hw[MONITOR_REG3] = mask;
    monitor_print_debug("[monitor-hw] set trigger mask to %d\n", mask);

}

/*
 * Monitor set AXI mask function
 *
 * @mask : AXI triggering mask
 *
 * This function sets a mask used to decide which AXI communication triggers the monitor execution.
 *
 */
void monitor_hw_set_axi_mask(int mask) {

    monitor_hw[MONITOR_REG2] = mask;
    monitor_hw[MONITOR_REG0] = MONITOR_AXI_SNIFFER_ENABLE_IN;
    monitor_print_debug("[monitor-hw] set AXI trigger mask to %d\n", mask);

}

/*
 * Monitor get acquisition time function
 *
 * This function gets the acquisition elapsed cycles used for data plotting in post-processing.
 *
 * Return : Elapsed cycles
 *
 */
int monitor_hw_get_time() {

    return monitor_hw[MONITOR_REG1];

}

/*
 * Monitor get power measurements function
 *
 * This function gets the number of power consumption measurements stored in the BRAM used in post-processing.
 *
 * Return : Number of power consupmtion measurements
 *
 */
int monitor_hw_get_number_power_measurements() {

    // +1 because the register hold the last written address (which is 0-indexed)
    return monitor_hw[MONITOR_REG2] + 1;

}

/*
 * Monitor get traces measurements function
 *
 * This function gets the number of probes events stored in the BRAM used in post-processing.
 *
 * Return : Number of probes events
 *
 */
int monitor_hw_get_number_traces_measurements() {

    // +1 because the register hold the last written address (which is 0-indexed)
    return monitor_hw[MONITOR_REG3] + 1;

}

/*
 * Monitor check sampling finished function
 *
 * This function checks if the monitor sampling process has finished.
 *
 * Return : True -> Sampling finished, False -> Sampling in process
 *
 */
int monitor_hw_isdone() {

    return ((monitor_hw[MONITOR_REG0] & MONITOR_DONE) > 0);

}

/*
 * Monitor check busy function
 *
 * This function checks if the monitor is busy.
 *
 * Return : True -> Busy, False -> Idle
 *
 */
int monitor_hw_isbusy() {

    return ((monitor_hw[MONITOR_REG0] & MONITOR_BUSY) > 0);

}

/*
 * Monitor get number of power measurement failed
 *
 * This function return the number of power samples gathered incorrectly from the ADC.
 *
 * Return : Number of errors
 *
 */
int monitor_hw_get_number_power_erros(){

    return (monitor_hw[MONITOR_REG0] >> MONITOR_POWER_ERRORS_OFFSET);

}
