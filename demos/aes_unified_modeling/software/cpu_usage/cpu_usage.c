/**
 * @file cpu_usage.c
 * @author Juan Encinas (juan.encinas@upm.es)
 * @brief This file contains the functions that calculate the cpu usage.
 * @date June 2023
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#include "cpu_usage.h"


// CPU usage measurement global variables
unsigned long long current_cpu_usage[CPU_USAGE_STAT_COLUMNS] = {0};
unsigned long long previous_cpu_usage[CPU_USAGE_STAT_COLUMNS] = {0};
float calculated_cpu_usage[CPU_USAGE_STAT_COLUMNS] = {0.0f};
pthread_t cpu_usage_pthread_id;
int cpu_usage_stop_flag = 0;


/**
 * @brief Add miliseconds to a timespec structure
 *
 * @param time Pointer to the timespec structure to be updated
 * @param msec Number of miliseconds to add to \p time
 */
void update_timer_ms(struct timespec *time, long int msec) {
    // Split ms into seconds and remaining milliseconds
    time->tv_sec += msec / 1000;         // Add full seconds from milliseconds
    long int remaining_ms = msec % 1000; // Remaining milliseconds

    // Convert remaining milliseconds to nanoseconds
    long int nsec = remaining_ms * 1000000L;

    // Add the nanoseconds to the timespec
    time->tv_nsec += nsec;

    // Handle overflow (more than a second)
    if (time->tv_nsec >= NS_PER_SECOND) {
        time->tv_sec += time->tv_nsec / NS_PER_SECOND;
        time->tv_nsec = time->tv_nsec % NS_PER_SECOND;
    }
    // Handle underflow (negative nanoseconds)
    else if (time->tv_nsec < 0) {
		printf("Error: Negative nanoseconds on update_times_ms\n");
        // time->tv_sec += (time->tv_nsec / NS_PER_SECOND) - 1;
        // time->tv_nsec = NS_PER_SECOND + (time->tv_nsec % NS_PER_SECOND);
    }
}

/**
 * @brief Print CPU usage header
 *
 * @param name Pointer to the structure describing the socket
 * @param path Path to the socket
 * @return (int) File descriptor of the socket on success, -1 otherwise
 */
void print_header() {
    printf("USER    NICE    SYS   IDLE   IOWAIT  IRQ  SOFTIRQ  STEAL\n");
}

/**
 * @brief Parse the /proc/stat "file" to gather CPU usage info
 *
 * @param cpu_stats Array to return CPU usage data by reference
 */
void cpu_usage_parse(unsigned long long* cpu_stats) {

    // Open the virtual /proc/stat file
    FILE* file = fopen(CPU_USAGE_STAT_FILE, "r");
    if (file == NULL) {
        perror("Failed to open CPU stat file\n");
        exit(1);
    }

    // Get info from the file
    if (fscanf(file, "cpu %llu %*llu %llu %llu",
               &cpu_stats[0], &cpu_stats[1], &cpu_stats[2]) != CPU_USAGE_STAT_COLUMNS) {
        printf("Unexpected /proc/stat format\n");
        exit(1);
    }

    // printf("CPU stats: %llu %llu %llu\n", cpu_stats[0], cpu_stats[1], cpu_stats[2]);

    // Close the file
    fclose(file);
}

/**
 * @brief Calculate CPU usage and update previous usage
 *
 * @param curr_stats Array of the latest CPU usage metrics
 * @param prev_stats Array of the previous CPU usage metrics
 * @param cpu_usage Array containing the calculate CPU usage ratio, returned by reference
 */
void calculate_and_update_cpu_usage(const unsigned long long* curr_stats, unsigned long long* prev_stats, float* cpu_usage) {

    unsigned long long total_time = 0;
    unsigned long long diff_stats[CPU_USAGE_STAT_COLUMNS];

    // Compute the difference of CPU jiffies within the time window
    // As well as update the previous stats
    for (int i = 0; i < CPU_USAGE_STAT_COLUMNS; i++) {
        diff_stats[i] = curr_stats[i] - prev_stats[i];
        prev_stats[i] = curr_stats[i];
        total_time += diff_stats[i];

        // printf("diff_stats[%d] = %llu\n", i, diff_stats[i]);
    }


    // Compute the percentual CPU usage
    for (int i = 0; i < CPU_USAGE_STAT_COLUMNS; i++) {
        if (total_time == 0)
            cpu_usage[i] = 0;
        else
            cpu_usage[i] = ((float)diff_stats[i] * 100) / total_time;
        // printf("cpu_usage[%d] = %f\n", i, cpu_usage[i]);
    }
}

/**
 * @brief Calculate CPU usage
 *
 * @param curr_stats Array of the latest CPU usage metrics
 * @param prev_stats Array of the previous CPU usage metrics
 * @param cpu_usage Array containing the calculate CPU usage ratio, returned by reference
 */
void calculate_cpu_usage(const unsigned long long* curr_stats, const unsigned long long* prev_stats, float* cpu_usage) {

    unsigned long long total_time = 0;
    unsigned long long diff_stats[CPU_USAGE_STAT_COLUMNS];

    // Compute the difference of CPU jiffies within the time window
    for (int i = 0; i < CPU_USAGE_STAT_COLUMNS; i++) {
        diff_stats[i] = curr_stats[i] - prev_stats[i];
        total_time += diff_stats[i];
    }

    // Compute the percentual CPU usage
    for (int i = 0; i < CPU_USAGE_STAT_COLUMNS; i++) {
        cpu_usage[i] = ((float)diff_stats[i] * 100) / total_time;
    }
}

/**
 * @brief Function executed by the cpu usage monitor thread.
 * 		  	- Periodically calculates the usage of the CPU by means of the
 * 			  user, kernel and idle time within a time window
 *
 * @param arg Pointer to the arguments passed to this function
 */
void* CPU_usage_monitor_thread(void *arg){

	struct timespec event_timer;
	//struct timespec t_aux;
	int period_in_ms = *((int*) arg);
    free(arg);

	// Start event timer
	clock_gettime(CLOCK_MONOTONIC, &event_timer);

	// Keep gathering CPU usage data until signaled
	while(cpu_usage_stop_flag == 0){

        // Wait for an event
        clock_nanosleep(CLOCK_MONOTONIC,TIMER_ABSTIME,&(event_timer), NULL);

        // Get actual CPU info
        cpu_usage_parse(current_cpu_usage);

        // Compute CPU usage and update CPU indo
        calculate_and_update_cpu_usage(current_cpu_usage, previous_cpu_usage, calculated_cpu_usage);

        // Update timer
        update_timer_ms(&event_timer, period_in_ms);

    }

	// Exit thread without return argument
	pthread_exit(NULL);
}

/**
 * @brief Start the CPU usage measurement
 *
 * @param period_in_ms Period in milliseconds to calculate the CPU usage
 * @return (float*) Pointer to the array containing the CPU usage
 */
float* start_CPU_usage_measurement(int period_in_ms){

    int ret;
    int *cpu_usage_monitor_period_ms = malloc(sizeof(int));
    *cpu_usage_monitor_period_ms = period_in_ms;
    cpu_usage_stop_flag = 0;

    // Create the thread that calculates the CPU usage
    ret = pthread_create(&cpu_usage_pthread_id, NULL, &CPU_usage_monitor_thread, cpu_usage_monitor_period_ms);
    if(ret != 0){
        printf("Error creating the cpu usage thread\n");
        exit(1);
    }
    printf("CPU usage thread created\n");

    return calculated_cpu_usage;
}

/**
 * @brief Stop the CPU usage measurement
 *
 * @return (int) 0 on success, -1 otherwise
 */
int stop_CPU_usage_measurement(){

    cpu_usage_stop_flag = 1;

    // Wait for the thread to finish
    pthread_join(cpu_usage_pthread_id, NULL);

    return 0;
}