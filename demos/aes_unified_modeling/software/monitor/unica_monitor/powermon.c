#include "powermon.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>


/* CONSTANTS */
#define VOLTAGE_SCALE 1.25
#define CURRENT_SCALE 1.25
#define POWER_SCALE 10

#define BYTE_SHIFT 8

#define COMMAND_LENGTH 100

#define BYTES_PER_BUFFER_LINE 6

#define ENABLE 1
#define DISABLE 0


/* PATHS */
char curr_en_path[] = "/sys/bus/iio/devices/iio:device1/scan_elements/in_current0_en";
char volt_en_path[] = "/sys/bus/iio/devices/iio:device1/scan_elements/in_voltage1_en";
char pow_en_path[] = "/sys/bus/iio/devices/iio:device1/scan_elements/in_power2_en";

char buffer_en_path[] = "/sys/bus/iio/devices/iio:device1/buffer/enable";
char buffer_len_path[] = "/sys/bus/iio/devices/iio:device1/buffer/length";
char buffer_data_available_path[] = "/sys/bus/iio/devices/iio:device1/buffer/data_available";

char volt_time_path[] = "/sys/bus/iio/devices/iio:device1/in_voltage1_integration_time";
char curr_time_path[] = "/sys/bus/iio/devices/iio:device1/in_current0_integration_time";
char sample_freq_path[] = "/sys/bus/iio/devices/iio:device1/in_sampling_frequency";

char device_path[] = "/dev/iio:device1";

/* GLOBAL VARIABLES */
struct timespec ts;
uint8_t* buffer = NULL; //pointer to dynamic array of unsigned bytes
int buffer_size;
char command[COMMAND_LENGTH];

double integration_time_available[] = {0.000140, 0.000204, 0.000332, 0.000588, 0.001100, 0.002116, 0.004156, 0.008244};



/* UTILITY FUNCTIONS SIGNATURES */
void echo_write(int val, char path[]);
void echo_write_float(double val, char path[]);
void echo_c(char phrase[]);
//void cat_c(char path[]);
int get_sysfs_value(char file_path[]);
void put_sysfs_value(int value, char file_path[]);


/* INTERNAL FUNCTIONS SIGNATURES */
void read_buffer();





/* LIBRARY FUNCTIONS */
void adc_setup(adc_config * cfg){

	// Write buffer length to file
	echo_write(cfg->buffer_size, buffer_len_path);
	printf("Selected %d element as buffer length\n", cfg->buffer_size);
	//cat_c(buffer_len_path); //verify actual written value

	// Write voltage sampling period to file
    if(cfg->voltage_sample_time >= 0 && cfg->voltage_sample_time <=7){
        echo_write_float(integration_time_available[cfg->voltage_sample_time], volt_time_path);
        printf("Selected %.3f ms as sampling interval for voltage\n", integration_time_available[cfg->voltage_sample_time] * 1000.0);
        //cat_c(volt_time_path); //verify actual written value
    }
    else{
        //printf("Incorrect value for voltage sampling time! Default value 1.1 ms selected\n");
	echo_write_float(integration_time_available[4], volt_time_path);
    }



	// Write current sampling period to file
    if(cfg->current_sample_time >=0 && cfg->current_sample_time <=7){
        echo_write_float(integration_time_available[cfg->current_sample_time], curr_time_path);
        printf("Selected %.3f ms as sampling interval for current\n", integration_time_available[cfg->current_sample_time] * 1000.0);
        //cat_c(curr_time_path); //verify actual written value
    }
    else{
        printf("Incorrect value for current sampling time! Default value 1.1 ms selected\n");
	echo_write_float(integration_time_available[4], curr_time_path);
    }



	// Show current sampling frequency
	echo_c("Sampling frequency:\n");
	//cat_c(sample_freq_path);


	printf("buff length selected = %d\n",cfg->buffer_size);
    //cat_c(buffer_len_path);
}



long long int start_monitor(){
    echo_write(DISABLE, buffer_en_path);
    //Enable scan element
    printf("Monitoring enabled\n");
    echo_write(ENABLE, curr_en_path);
    //cat_c(curr_en_path); //verify
    echo_write(ENABLE, volt_en_path);
    //cat_c(volt_en_path);
    echo_write(ENABLE, pow_en_path);
    //cat_c(pow_en_path);

	// Starting timestamp
    long long int time;
	clock_gettime(CLOCK_REALTIME, &ts);
    time = ((ts.tv_sec * 1000000000)+ ts.tv_nsec);

    struct timespec time_;
    clock_gettime(CLOCK_MONOTONIC, &time_);
    printf("unica Monitor started at %ld:%ld\n", time_.tv_sec, time_.tv_nsec);

	// Enable buffer in linux
	echo_write(ENABLE, buffer_en_path);

	return time;
}


long long int stop_monitor(){
	// Disable buffer
	echo_write(DISABLE, buffer_en_path);

	// Ending timestamp
    long long int time;
    clock_gettime(CLOCK_REALTIME, &ts);
    time = ((ts.tv_sec * 1000000000)+ ts.tv_nsec);

    struct timespec time_;
    clock_gettime(CLOCK_MONOTONIC, &time_);
    printf("unica Monitor STOP at %ld:%ld\n", time_.tv_sec, time_.tv_nsec);

    // Disable scan elements
    printf("Monitoring disabled\n");
    echo_write(DISABLE, curr_en_path);
    //cat_c(curr_en_path); //verify
    echo_write(DISABLE, volt_en_path);
    //cat_c(volt_en_path); //verify
    echo_write(DISABLE, pow_en_path);
    //cat_c(pow_en_path); //verify
    printf("Actual number of samples = %d\n", get_sysfs_value(buffer_data_available_path));

    read_buffer();

	return time;
}


void print_buffer(adc_config * cfg){

    // Print processed data
    printf("\n");
    printf("Printing processed buffer data:\n");

    // Index moves line by line
    for (int i = 0; i < buffer_size; i += BYTES_PER_BUFFER_LINE) {
        printf("Set %d: ", i/BYTES_PER_BUFFER_LINE + 1);

        // Little-endian to big-endian conversion, data processing and printing
        if(cfg->curr_en){
            printf("Current: %.2f mA  ", (buffer[i] | (buffer[i + 1] << BYTE_SHIFT)) * CURRENT_SCALE);
        }
        if(cfg->volt_en){
            printf("Voltage: %.2f mV  ", (buffer[i + 2] | (buffer[i + 3] << BYTE_SHIFT)) * VOLTAGE_SCALE);
        }
        if(cfg->pow_en){
            printf("Power: %d mW  ", (buffer[i + 4] | (buffer[i + 5] << BYTE_SHIFT)) * POWER_SCALE);
        }
        printf("\n");
    }
}


void free_memory(){
    free(buffer);
    buffer = NULL;
    printf("Memory deallocation complete\n");
}





/* INTERNAL FUNCTIONS */

/* Read from real buffer and copy data in a dynamically allocated array */
void read_buffer() {

    // Read number of buffer lines
    int buff_data_available = get_sysfs_value(buffer_data_available_path);

    // Calculate buffer size in bytes
    buffer_size = buff_data_available * BYTES_PER_BUFFER_LINE * (int)sizeof(uint8_t);

    // Allocate array memory in bytes
    buffer = (uint8_t *) malloc(buffer_size);
    if (buffer == NULL) {
        perror("Error allocating memory");
        exit(EXIT_FAILURE);
    }

    // Open device file (real buffer data)
    int fd = open(device_path, O_RDONLY);
    if (fd == -1) {
        perror("Error opening the file");
        exit(EXIT_FAILURE);
    }

    // Read file and store content in buffer array
    ssize_t bytesRead = read(fd, buffer, buffer_size);
    if (bytesRead == -1) {
        perror("Error reading the file");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
}





/* UTILITY FUNCTIONS */

/* Execute echo command in linux CLI to write an integer value to a file */
void echo_write(int val, char file_path[]){
    sprintf(command, "echo %d > %s", val, file_path);
    system(command);
}

/* Execute echo command in linux CLI to write a floating point value to a file */
void echo_write_float(double val, char file_path[]){
    sprintf(command, "echo %.6f > %s", val, file_path);
    system(command);
}

/* Execute echo command in linux CLI to print a phrase */
void echo_c(char phrase[]){
    sprintf(command, "echo %s", phrase);
    system(command);
}

/* Execute cat command in linux CLI to print the content of a file */
/*
void cat_c(char file_path[]){
    sprintf(command, "cat %s", file_path);
    system(command);
}
*/

/* Return an integer value read from a file in the linux file system */
int get_sysfs_value(char file_path[]) {
    FILE *file = fopen(file_path, "r");
    if (file == NULL) return -1;

    int value;
    if (fscanf(file, "%d", &value) != 1) {
        fclose(file);
        return -1;
    }

    fclose(file);
    return value;
}

void put_sysfs_value(int value, char file_path[]){
    // Open the file
    FILE *file = fopen(file_path, "w");
    if (file == NULL){
        exit(EXIT_FAILURE);
    }

    // Write value on file
    fprintf(file,"%d", value);

    // Close the file
    fclose(file);;
}

void write_on_files(){
    // Open files
    FILE *current_txt = fopen("current.txt", "w");
    FILE *voltage_txt = fopen("voltage.txt", "w");
    FILE *power_txt = fopen("power.txt", "w");

    // Check correct file opening
    if(current_txt == NULL || voltage_txt == NULL || power_txt == NULL){
        printf("Error opening the files\n");
        return;
    }

    for (int i = 0; i < buffer_size; i += BYTES_PER_BUFFER_LINE) {
        // Little-endian to big-endian conversion, data processing, file writing

        fprintf(current_txt, "%.2f\n", (buffer[i] | (buffer[i + 1] << BYTE_SHIFT)) * CURRENT_SCALE);

        fprintf(voltage_txt, "%.2f\n", (buffer[i + 2] | (buffer[i + 3] << BYTE_SHIFT)) * VOLTAGE_SCALE);

        fprintf(power_txt, "%d\n", (buffer[i + 4] | (buffer[i + 5] << BYTE_SHIFT)) * POWER_SCALE);
    }
    fclose(current_txt);
    fclose(voltage_txt);
    fclose(power_txt);
}


void write_on_files_append(){
	// Open files
	FILE *current_txt = fopen("current_append.txt", "a");
	FILE *voltage_txt = fopen("voltage_append.txt", "a");
	FILE *power_txt = fopen("power_append.txt", "a");

	// Check correct file opening
	if(current_txt == NULL || voltage_txt == NULL || power_txt == NULL){
	    printf("Error opening the files\n");
	    return;
	}

	for(int i = 0; i < buffer_size; i += BYTES_PER_BUFFER_LINE) {
	    // Little-endian to big-endian conversion, data processing, file writing
	    fprintf(current_txt, "%.2f\n", (buffer[i] | (buffer[i + 1] << BYTE_SHIFT)) * CURRENT_SCALE);
	    fprintf(voltage_txt, "%.2f\n", (buffer[i + 2] | (buffer[i + 3] << BYTE_SHIFT)) * VOLTAGE_SCALE);
	    fprintf(power_txt, "%d\n", (buffer[i + 4] | (buffer[i + 5] << BYTE_SHIFT)) * POWER_SCALE);
	}
	fclose(current_txt);
	fclose(voltage_txt);
	fclose(power_txt);
}


// Juan
int get_power(int ** power_buffer) {

    int power_buffer_length = buffer_size / (BYTES_PER_BUFFER_LINE); // Number of power samples

    *power_buffer = (int *) malloc(power_buffer_length * sizeof(int));

    for (int i = 0, j = 0; i < buffer_size; i += BYTES_PER_BUFFER_LINE, j++) {
        // Little-endian to big-endian conversion, data processing, file writing
        (*power_buffer)[j] = (buffer[i + 4] | (buffer[i + 5] << BYTE_SHIFT)) * POWER_SCALE;
    }

    return power_buffer_length;
}