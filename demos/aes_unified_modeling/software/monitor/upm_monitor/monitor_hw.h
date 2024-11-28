/*
 * Monitor low-level hardware API
 *
 * Author      : Juan Encinas Anchústegui <juan.encinas@upm.es>
 * Date        : February 2021
 * Description : This file contains the low-level functions required to
 *               work with the Monitor infrastructure (Monitor registers).
 *
 */


#ifndef _MONITOR_HW_H_
#define _MONITOR_HW_H_

extern uint32_t *monitor_hw;

#define MONITOR_POWER_ADDR  (0xb0100000)
#define MONITOR_TRACES_ADDR (0xb0180000)

/*
 * Monitor infrastructure register offsets (in 32-bit words)
 *
 */
#define MONITOR_REG0    (0x00000000 >> 2)                     // REG 0
#define MONITOR_REG1    (0x00000004 >> 2)                     // REG 1
#define MONITOR_REG2    (0x00000008 >> 2)                     // REG 2
#define MONITOR_REG3    (0x0000000c >> 2)                     // REG 3

/*
 * Monitor infrastructure commands
 *
 */
#define MONITOR_CONFIG_VREF             0x01    // In
#define MONITOR_CONFIG_2VREF            0x02    // In
#define MONITOR_START                   0x04    // In
#define MONITOR_STOP                    0x08    // In
#define MONITOR_AXI_SNIFFER_ENABLE_IN   0x20    // In
#define MONITOR_BUSY                    0x01    // Out
#define MONITOR_DONE                    0x02    // Out
#define MONITOR_AXI_SNIFFER_ENABLE_OUT  0x04    // Out
#define MONITOR_POWER_ERRORS_OFFSET     0x03    // Offset


struct monitorRegion_t {
    char *name;
    size_t size;
    void *data;
};

struct monitorData_t {
    struct monitorRegion_t *power;
    struct monitorRegion_t *traces;
};

/*
 * Monitor normal voltage reference configuration function
 *
 * This function sets the monitor ADC voltage reference to 2.5V.
 *
 */
void monitor_hw_config_vref();

/*
 * Monitor double voltage reference configuration function
 *
 * This function sets the monitor ADC voltage reference to 5V.
 *
 */
void monitor_hw_config_2vref();

/*
 * Monitor start function
 *
 * This function starts the monitor acquisition.
 *
 */
void monitor_hw_start();

/*
 * Monitor clean function
 *
 * This function cleans the monitor memory banks.
 *
 */
void monitor_hw_clean();

/*
 * Monitor stop function
 *
 * This function stop the monitor acquisition. (only makes sense when power monitoring disabled)
 *
 */
void monitor_hw_stop();

/*
 * Monitor set mask function
 *
 * @mask : Triggering mask
 *
 * This function sets a mask used to decide which signals trigger the monitor execution.
 *
 */
void monitor_hw_set_mask(int mask);

/*
 * Monitor set AXI mask function
 *
 * @mask : AXI triggering mask
 *
 * This function sets a mask used to decide which AXI communication triggers the monitor execution.
 *
 */
void monitor_hw_set_axi_mask(int mask);

/*
 * Monitor get acquisition time function
 *
 * This function gets the acquisition elapsed cycles used for data plotting in post-processing.
 *
 * Return : Elapsed cycles
 *
 */
int monitor_hw_get_time();

/*
 * Monitor get power measurements function
 *
 * This function gets the number of power consumption measurements stored in the BRAM used in post-processing.
 *
 * Return : Number of power consupmtion measurements
 *
 */
int monitor_hw_get_number_power_measurements();

/*
 * Monitor get traces measurements function
 *
 * This function gets the number of probes events stored in the BRAM used in post-processing.
 *
 * Return : Number of probes events
 *
 */
int monitor_hw_get_number_traces_measurements();

/*
 * Monitor get acquisition time function
 *
 * This function gets the acquisition elapsed cycles used for data plotting in post-processing.
 *
 * Return : Elapsed cycles
 *
 */
int monitor_hw_isdone();

/*
 * Monitor check busy function
 *
 * This function checks if the monitor is busy.
 *
 * Return : True -> Busy, False -> Idle
 *
 */
int monitor_hw_isbusy();

/*
 * Monitor get number of power measurement failed
 *
 * This function return the number of power samples gathered incorrectly from the ADC.
 *
 * Return : Number of errors
 *
 */
int monitor_hw_get_number_power_erros();

#endif /* _MONITOR_HW_H_ */
