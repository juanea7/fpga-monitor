/*
 * UPM + UNICA Monitor
 *
 * Author      : Juan Encinas Anchústegui <juan.encinas@upm.es>
 * Date        : November 2024
 * Description : This file contains the unified UPM + UNICA Monitor runtime API, which can
 *               be used by any application to monitor the power consumption and performances
 *               of hardware accelerators.
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#include "upm_monitor/monitor.h"
#include "unica_monitor/powermon.h"


/* UPM monitor */
#define TRACES_SAMPLES (64)

// [UPM] variables
monitortdata_t *traces;

long long int time_before, time_after;
/*
 * Unified Monitor init function
 *
 * This function initialized the UPM and UNICA monitoring infrastructure.
 *
 */
int unified_monitor_init() {

    // [UNICA] structure for monitoring setup
	adc_config config = {
		.buffer_size = 5000,
		.voltage_sample_time = 0,
		.current_sample_time = 0,
	};
	adc_config* cfg_pointer = &config;

	// [UNICA] adc setup
	adc_setup(cfg_pointer);

    // [UPM] Load Monitor overlay and driver
    printf("Monitor linux setup return: %d\n",system("./setup_monitor/setup_monitor.sh"));

    // [UPM] Initialize Monitor infrastructure
    monitor_init();

    // [UPM] Allocate data buffers for MONITOR
    traces = monitor_alloc(TRACES_SAMPLES, "traces", MONITOR_REG_TRACES);

    return 0;
}

/*
 * Unified Monitor start function
 *
 * This function starts the UPM and UNICA monitoring infrastructure.
 *
 */
int unified_monitor_start() {

	// [UNICA] monitor start
	time_before = start_monitor();
    // [UPM] Send start command to MONITOR
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    monitor_start();
    printf("Monitor started at %ld:%ld\n", time.tv_sec, time.tv_nsec);
    return 0;
}

/*
 * Unified Monitor stop function
 *
 * This function stops the UPM and UNICA monitoring infrastructure.
 *
 */
int unified_monitor_stop() {

    // [UPM] Wait for the interruption that signals the end of the monitor execution
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    monitor_stop();
    printf("Monitor stopped at %ld:%ld\n", time.tv_sec, time.tv_nsec);
	// [UNICA] stop monitor
	time_after = stop_monitor();

	printf("\nTime elapsed: %f ms\n", ((double)time_after - (double)time_before)/1000000);

    return 0;
}

/*
 * Unified Monitor read function
 *
 * This function reads the UPM and UNICA monitoring data.
 *
 */
int unified_monitor_read() {

	// [UPM] variables
    int fd_power, fd_traces, iter;

	/* [UNICA] Print data buffer */
	// config.curr_en = 0;
	// config.volt_en = 0;
	// config.pow_en = 1;
	// print_buffer(cfg_pointer);

	// [UNICA] Get power data
	int *power_buffer;
	int power_buffer_lenght = get_power(&power_buffer);

    // [UNICA] Free memory
	free_memory();

	// [UPM] Read number traces measurements stored in the BRAMS
    unsigned int number_traces_samples = monitor_get_number_traces_measurements();
    // printf("Number of traces samples : \t%d\n", number_traces_samples);

    if (monitor_read_traces(number_traces_samples + number_traces_samples%4) != 0){
        printf("Error reading traces\n\n\r");
    goto monitor_err;
    }

    // for(iter = 0; iter < 5; iter++){
    //     printf("| Trace #%2d : Time -> %10u | Signals -> %10llu |\n\r", iter, traces[iter] & 0xffffffff, traces[iter] >> 32);
    // }
    // printf("\n\n---------------------------------------\n\n\r");

    unsigned elapsed_time = monitor_get_time();
    printf("Elapsed time : \t%d\n\r", elapsed_time);

    // [UPM] Store power and traces for further processing
    fd_power = open("CON.BIN", O_WRONLY | O_CREAT, 0644);
    if(fd_power< 0){
        printf("Error! CON file cannot be opened.\n\n");
    goto monitor_err;
    }
    // Store traces for further processing
    fd_traces = open("SIG.BIN", O_WRONLY | O_CREAT, 0644);
    if(fd_traces < 0){
        printf("Error! SIG file cannot be opened.\n\n");
    goto trace_open_err;
    }

    // [UPM + UNICA] Write data to files
    write(fd_power, power_buffer, power_buffer_lenght * sizeof(int)); // stores power buffer at the end of the CON.BIN file
    write(fd_power, &elapsed_time, sizeof(elapsed_time));   // stores elapsed time at the end of the CON.BIN file
    write(fd_traces, traces, sizeof(monitortdata_t) * number_traces_samples);

    // [UPM] Close file descriptors
    trace_open_err:
    close(fd_traces);

    monitor_err:
	close(fd_power);

    // [UPM] Clean monitor HW
    monitor_clean();

    return 0;
}

/*
 * Unified Monitor clean function
 *
 * This function cleans the UPM and UNICA monitoring infrastructure.
 *
 */
int unified_monitor_clean() {

    // [UPM] Free monitor data buffers
    monitor_free("traces");
    // [UPM] Clean monitor setup
    monitor_exit();

    return 0;
}