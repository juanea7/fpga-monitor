#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <sys/mman.h>
#include <stdlib.h>
#include "dma_simplemode.h"



/* write_dma(): write a value into memory
 *
 *	unsigned int *virtual_addr: the address in which value should be written
 *	int offset:					offset of virtual_addr base address
 *	unsigned int value:			the value to write into memory
 *
 * 	return 0 if the writing is ok
 */
unsigned int write_dma(unsigned int *virtual_addr, int offset, unsigned int value)
{
	virtual_addr[offset >> 2] = value;

	return 0;
}

/* read_dma(): read a value from memory
 *
 *	unsigned int *virtual_addr: the address in which value should be read
 *	int offset:					offset of virtual_addr base address
 *
 * 	return the read value
 */
unsigned int read_dma(unsigned int *virtual_addr, int offset)
{
	return virtual_addr[offset >> 2];
}

/* dma_s2mm_status(): print the status of dma_s2mm
 *
 *	print the s2mm status register
 *
 * 	return void
 */
void dma_s2mm_status(unsigned int *virtual_addr)
{
	// read the status
	// S2MM_STATUS_REGISTER --> offset of S2MM status register
	unsigned int status = read_dma(virtual_addr, S2MM_STATUS_REGISTER);

	printf("Stream to memory-mapped status (0x%08x@0x%02x):", status, S2MM_STATUS_REGISTER);

	if (status & STATUS_HALTED)
	{
		printf(" Halted.\n");
	}
	else
	{
		// printf(" Running.\n");
	}

	if (status & STATUS_IDLE)
	{
		printf(" Idle.\n");
	}

	if (status & STATUS_SG_INCLDED)
	{
		printf(" SG is included.\n");
	}

	if (status & STATUS_DMA_INTERNAL_ERR)
	{
		printf(" DMA internal error.\n");
	}

	if (status & STATUS_DMA_SLAVE_ERR)
	{
		printf(" DMA slave error.\n");
	}

	if (status & STATUS_DMA_DECODE_ERR)
	{
		printf(" DMA decode error.\n");
	}

	if (status & STATUS_SG_INTERNAL_ERR)
	{
		printf(" SG internal error.\n");
	}

	if (status & STATUS_SG_SLAVE_ERR)
	{
		printf(" SG slave error.\n");
	}

	if (status & STATUS_SG_DECODE_ERR)
	{
		printf(" SG decode error.\n");
	}

	if (status & STATUS_IOC_IRQ)
	{
		printf(" IOC interrupt occurred.\n");
	}

	if (status & STATUS_DELAY_IRQ)
	{
		printf(" Interrupt on delay occurred.\n");
	}

	if (status & STATUS_ERR_IRQ)
	{
		printf(" Error interrupt occurred.\n");
	}
}

/* dma_mm2s_status(): print the status of dma_mm2s
 *
 *	print the mm2s status register
 *
 * 	return void
 */
void dma_mm2s_status(unsigned int *virtual_addr)
{

	// read the status
	// MM2S_STATUS_REGISTER --> offset of MM2S status register
	unsigned int status = read_dma(virtual_addr, MM2S_STATUS_REGISTER);

	// printf("Memory-mapped to stream status (0x%08x@0x%02x):", status, MM2S_STATUS_REGISTER);

	if (status & STATUS_HALTED)
	{
		printf(" Halted.\n");
	}
	else
	{
		// printf(" Running.\n");
	}

	if (status & STATUS_IDLE)
	{
		printf(" Idle.\n");
	}

	if (status & STATUS_SG_INCLDED)
	{
		printf(" SG is included.\n");
	}

	if (status & STATUS_DMA_INTERNAL_ERR)
	{
		printf(" DMA internal error.\n");
	}

	if (status & STATUS_DMA_SLAVE_ERR)
	{
		printf(" DMA slave error.\n");
	}

	if (status & STATUS_DMA_DECODE_ERR)
	{
		printf(" DMA decode error.\n");
	}

	if (status & STATUS_SG_INTERNAL_ERR)
	{
		printf(" SG internal error.\n");
	}

	if (status & STATUS_SG_SLAVE_ERR)
	{
		printf(" SG slave error.\n");
	}

	if (status & STATUS_SG_DECODE_ERR)
	{
		printf(" SG decode error.\n");
	}

	if (status & STATUS_IOC_IRQ)
	{
		printf(" IOC interrupt occurred.\n");
	}

	if (status & STATUS_DELAY_IRQ)
	{
		printf(" Interrupt on delay occurred.\n");
	}

	if (status & STATUS_ERR_IRQ)
	{
		printf(" Error interrupt occurred.\n");
	}
}

/* dma_mm2s_sync(): wait for mm2s synchronization
 *
 *	print the mm2s status register
 *
 * 	return void
 */
int dma_mm2s_sync(unsigned int *virtual_addr)
{
	// read dma MM2S status register
	unsigned int mm2s_status = read_dma(virtual_addr, MM2S_STATUS_REGISTER);

	// sit in this while loop as long as the status does not read back 0x00001002 (4098)
	//
	// 0x00001002 = IOC interrupt has occured and DMA is idle
	// #define IOC_IRQ_FLAG                1<<12	// 0001 0000 0000 0000 = 00001000
	// #define IDLE_FLAG                   1<<1		// 0000 0000 0000 0010 = 00000002

	while (!(mm2s_status & IOC_IRQ_FLAG) || !(mm2s_status & IDLE_FLAG)) // and bitwise
	{
		printf("dma_mm2s_sync-------\n");
		dma_s2mm_status(virtual_addr);
		dma_mm2s_status(virtual_addr);

		mm2s_status = read_dma(virtual_addr, MM2S_STATUS_REGISTER);
	}
	printf("dma_mm2s_sync-------\n");
	dma_s2mm_status(virtual_addr);
	dma_mm2s_status(virtual_addr);
	return 0;
}

int dma_s2mm_sync(unsigned int *virtual_addr)
{
	unsigned int s2mm_status = read_dma(virtual_addr, S2MM_STATUS_REGISTER);

	// sit in this while loop as long as the status does not read back 0x00001002 (4098)
	// 0x00001002 = IOC interrupt has occured and DMA is idle
	while (!(s2mm_status & IOC_IRQ_FLAG) || !(s2mm_status & IDLE_FLAG))
	{
		dma_mm2s_status(virtual_addr);

		s2mm_status = read_dma(virtual_addr, S2MM_STATUS_REGISTER);
	}

	return 0;
}

void print_mem(void *virtual_address, int byte_count)
{
	char *data_ptr = virtual_address;

	for (int i = 0; i < byte_count; i++)
	{
		printf("%02X", data_ptr[i]);

		// print a space every 4 bytes (0 indexed)
		if (i % 4 == 3)
		{
			printf(" ");
		}
	}

	printf("\n");
}
