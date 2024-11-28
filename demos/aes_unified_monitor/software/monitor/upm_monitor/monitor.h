/*
 * Monitor runtime API
 *
 * Author      : Juan Encinas Anchústegui <juan.encinas@upm.es>
 * Date        : February 2021
 * Description : This file contains the Monitor runtime API, which can
 *               be used by any application to get access to adaptive
 *               hardware acceleration.
 *
 */


#ifndef _MONITOR_H_
#define _MONITOR_H_

#include <stdlib.h> // size_t
#include <stdint.h> // uint32_t

/*
 * Monitor data type
 *
 * This is the main data type to be used when creating buffers between
 * user applications and monitor hardware memory banks. All memory banks to be
 * accessed need to be declared as pointers to this type.
 *
 *     monitorpdata_t *power  = monitor_alloc(ndata, regname, MONITOR_REG_POWER);
 *     monitortdata_t *traces = monitor_alloc(ndata, regname, MONITOR_REG_TRACES);
 *
 */
typedef uint32_t monitorpdata_t;
typedef uint64_t monitortdata_t;


/*
 * MONITOR region type
 *
 * MONITOR_REG_POWER  - Monitor power data region
 * MONITOR_REG_TRACES - Monitor traces data region
 *
 */
enum monitorregtype_t {MONITOR_REG_POWER, MONITOR_REG_TRACES};


/*
 * SYSTEM INITIALIZATION
 *
 */

/*
 * Monitor init function
 *
 * This function sets up the basic software entities required to manage
 * the Monitor low-level functionality (DMA transfers, registers access, etc.).
 *
 * Return : 0 on success, error code otherwise
 */
int monitor_init();


/*
 * Monitor exit function
 *
 * This function cleans the software entities created by monitor_init().
 *
 */
void monitor_exit();


/*
 * SYSTEM MANAGEMENT
 *
 */


/*
 * Monitor normal voltage reference configuration function
 *
 * This function sets the monitor ADC voltage reference to 2.5V.
 *
 */
void monitor_config_vref();


/*
 * Monitor double voltage reference configuration function
 *
 * This function sets the monitor ADC voltage reference to 5V.
 *
 */
void monitor_config_2vref();

/*
 * Monitor start function
 *
 * This function starts the monitor acquisition.
 *
 */
void monitor_start();

/*
 * Monitor clean function
 *
 * This function cleans the monitor memory banks.
 *
 */
void monitor_clean();

/*
 * Monitor stop function
 *
 * This function stop the monitor acquisition. (only makes sense when power monitoring disabled)
 *
 */
void monitor_stop();

/*
 * Monitor set mask function
 *
 * @mask : Triggering mask
 *
 * This function sets a mask used to decide which signals trigger the monitor execution.
 *
 */
void monitor_set_mask(int mask);

/*
 * Monitor set AXI mask function
 *
 * @mask : AXI triggering mask
 *
 * This function sets a mask used to decide which AXI communication triggers the monitor execution.
 *
 */
void monitor_set_axi_mask(int mask);

/*
 * Monitor get acquisition time function
 *
 * This function gets the acquisition elapsed cycles used for data plotting in post-processing.
 *
 * Return : Elapsed cycles
 *
 */
int monitor_get_time();

/*
 * Monitor get power measurements function
 *
 * This function gets the number of power consumption measurements stored in the BRAM used in post-processing.
 *
 * Return : Number of power consupmtion measurements
 *
 */
int monitor_get_number_power_measurements();

/*
 * Monitor get traces measurements function
 *
 * This function gets the number of probes events stored in the BRAM used in post-processing.
 *
 * Return : Number of probes events
 *
 */
int monitor_get_number_traces_measurements();

/*
 * Monitor is done function
 *
 * This function checks if the acquisition has finished.
 *
 * Return : 1 if acquisition finished, 0 otherwise
 *
 */
int monitor_isdone();

/*
 * Monitor is busy function
 *
 * This function checks if the monitor is busy.
 *
 * Return : 1 if busy, 0 if idle
 *
 */
int monitor_isbusy();

/*
 * Monitor get number of power errors function
 *
 * This function return the number of incorrect power samples received from ADC
 *
 * Return : Number of errors [0,$number_power_measurements]
 *
 */
int monitor_get_power_errors();


/*
 * Monitor no busy-wait waiting function
 *
 * This function waits for the monitor to finish in a not busy-wait manner.
 *
 */
void monitor_wait();


/*
 * Monitor power consumption read function
 *
 * This function reads the monitor power consumption data sampled.
 *
 * @ndata  	: amount of data to be read from power memory bank
 *
 * Return : 0 on success, error code otherwise
 *
 */
int monitor_read_power_consumption(unsigned int ndata);


/*
 * Monitor traces read function
 *
 * This function reads the monitor traces data sampled.
 *
 * @ndata  	: amount of data to be read from traces memory bank
 *
 * Return : 0 on success, error code otherwise
 *
 */
int monitor_read_traces(unsigned int ndata);


/*
 * Monitor allocate buffer memory
 *
 * This function allocates dynamic memory to be used as a buffer between
 * the application and the local memories in the hardware kernels.
 *
 * @ndata  	: amount of data to be allocated for the buffer
 * @regname : memory bank name to associate this buffer with
 * @regtype : memory bank type (power or traces)
 *
 * Return : pointer to allocated memory on success, NULL otherwise
 *
 */
void *monitor_alloc(int ndata, const char *regname, enum monitorregtype_t regtype);


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
int monitor_free(const char *regname);


#endif /* _MONITOR_H_ */
