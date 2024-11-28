#ifndef POWERMON_H
#define POWERMON_H

#include <stdbool.h>
#include <stdint.h>


/* adc_config
 * structure to pass configuration to adc
 * must be initialised in main()
 *
 * *_sample_ratio: select a sampling time interval from the list below:
 * 0 = 140 us, 1 = 204 us, 2 = 332 us, 3 = 588 us, 4 = 1.100 ms, 5 = 2.116 ms, 6 = 4.156 ms, 7 = 8.244 ms
 *
 * buffer_size
 * predicted buffer size, it must be chosen in relation to program time execution
 * and sampling frequency
 *
 * curr_en, volt_en, pow_en
 * boolean variable to enable/disable element to print
 * 0 = disable, 1 = enable
 *
 */
typedef struct {
    bool curr_en;
    bool volt_en;
    bool pow_en;
    int buffer_size;
    int voltage_sample_time;
    int current_sample_time;
} adc_config;


/* setup_adc
 *
 * Setup the adc
 *
 * requires a pointer to struct adc_config
 *
 */
void adc_setup(adc_config*);



/* start_monitor
 *
 * start buffer capture
 *
 * stop_monitor
 * function to stop buffer capture
 *
 * these functions return the time instant the procedure starts and stops
 * two long long int variable to store the timestamps must be declared in the main()
 *
 */

long long int start_monitor();

long long int stop_monitor();


/* print_buffer
 *
 * print the content of the buffer
 * pointer to struct adc_config is needed to choose which measure to print
 *
 * buffer is available until the function free_memory() is called
 */

void print_buffer(adc_config*);

/* free_memory
 *
 * deallocate buffer memory
 * buffer reading operation allocates memory in the system so that the data can be accessible
 * multiple times
 * if the data is no longer needed call this function to free the memory
 */
void free_memory();


/* write_on_files
 *
 * create files current.txt, voltage.txt, power.txt
 * if file already exist they will be overwritten
 * files contain processed data of each mesure
 */
void write_on_files();

/* write_on_files_append
 *
 * create files curr_append.txt, volt_append.txt power_append.txt
 * if file already exist it will append data
 */
void write_on_files_append();



// Juan
int get_power(int ** power_buffer);

#endif //POWERMON_H
