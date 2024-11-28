/**
 * @file cpu_usage.h
 * @author Juan Encinas (juan.encinas@upm.es)
 * @brief This file contains the functions that calculate the cpu usage.
 * @date June 2023
 *
 */

#ifndef _CPU_USAGE_H_
#define _CPU_USAGE_H_

#define CPU_USAGE_STAT_FILE "/proc/stat"
#define CPU_USAGE_STAT_COLUMNS 3

// Nanosecond in a second
#define NS_PER_SECOND 1000000000L

/**
 * @brief Add miliseconds to a timespec structure
 *
 * @param time Pointer to the timespec structure to be updated
 * @param msec Number of miliseconds to add to \p time
 */
void update_timer_ms(struct timespec *time, long int msec);

/**
 * @brief Print CPU usage header
 *
 * @param name Pointer to the structure describing the socket
 * @param path Path to the socket
 * @return (int) File descriptor of the socket on success, -1 otherwise
 */
void print_header();

/**
 * @brief Parse the /proc/stat "file" to gather CPU usage info
 *
 * @param cpu_stats Array to return CPU usage data by reference
 */
void cpu_usage_parse(unsigned long long* cpu_stats);

/**
 * @brief Calculate CPU usage
 *
 * @param curr_stats Array of the latest CPU usage metrics
 * @param prev_stats Array of the previous CPU usage metrics
 * @param cpu_usage Array containing the calculate CPU usage ratio, returned by reference
 */
void calculate_and_update_cpu_usage(const unsigned long long* curr_stats, unsigned long long* prev_stats, float* cpu_usage);

/**
 * @brief Calculate CPU usage
 *
 * @param curr_stats Array of the latest CPU usage metrics
 * @param prev_stats Array of the previous CPU usage metrics
 * @param cpu_usage Array containing the calculate CPU usage ratio, returned by reference
 */
void calculate_cpu_usage(const unsigned long long* curr_stats, const unsigned long long* prev_stats, float* cpu_usage);

/**
 * @brief Start the CPU usage measurement
 *
 * @param period_in_ms Period in milliseconds to calculate the CPU usage
 * @return (float*) Pointer to the array containing the CPU usage
 */
float* start_CPU_usage_measurement(int period_in_ms);

/**
 * @brief Stop the CPU usage measurement
 *
 * @return (int) 0 on success, -1 otherwise
 */
int stop_CPU_usage_measurement();


#endif /* _CPU_USAGE_H_ */