#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <sys/mman.h>
#include <stdlib.h>


#include "dma_simplemode.h"
#include "unified_monitor.h"


/* AES accelerator */
#define OFFS_AES_IP 	0xA0010000
#define SLV_REG0	0x00000000
#define OUT_SIZE_SHIFT	9
#define OUT_SIZE 4	//128 bits (output size) divided by 32 bit (stream word size)

#define TEXT_SIZE_BYTE 16
#define KEY_SIZE_BYTE 16
#define ENCRYPTED_SIZE_BYTE 16

/* Customizable buffer for input and output data */
#define TEXT_BUFFER	0x0E000000
#define KEY_BUFFER	0x0E010000
#define ENCRYPTED_BUFFER	0x0F000000



int main()
{

	/*  Configure the PL */
	system("fpgautil -b design_1_wrapper.bin");

	// Initialize monitor
	unified_monitor_init();

	/*  Open DDR memory */
	int ddr_memory = open("/dev/mem", O_RDWR | O_SYNC);

	printf(" DDR memory opened\n");

	/*  Map PL registers */
	unsigned int *aes_ip_virtual_addr = mmap(NULL, 65535, PROT_READ | PROT_WRITE, MAP_SHARED, ddr_memory, OFFS_AES_IP);
	unsigned int *dma_TEXT_virtual_addr = mmap(NULL, 65535, PROT_READ | PROT_WRITE, MAP_SHARED, ddr_memory, OFFS_DMA_TEXT_DATA);
	unsigned int *dma_KEY_virtual_addr = mmap(NULL, 65535, PROT_READ | PROT_WRITE, MAP_SHARED, ddr_memory, OFFS_DMA_KEY_DATA);
	unsigned int *dma_ENCRYPTED_virtual_addr = mmap(NULL, 65535, PROT_READ | PROT_WRITE, MAP_SHARED, ddr_memory, OFFS_DMA_ENCRYPT_DATA);

	printf(" PL registers mapped\n");

	/* Map data buffers */
	unsigned int *virtual_src_TEXT_addr = mmap(NULL, 65535, PROT_READ | PROT_WRITE, MAP_SHARED, ddr_memory, TEXT_BUFFER);
	unsigned int *virtual_src_KEY_addr = mmap(NULL, 65535, PROT_READ | PROT_WRITE, MAP_SHARED, ddr_memory, KEY_BUFFER);
	unsigned int *virtual_dst_ENCRYPTED_addr = mmap(NULL, 65535, PROT_READ | PROT_WRITE, MAP_SHARED, ddr_memory, ENCRYPTED_BUFFER);

	printf(" Data buffers mapped\n");

	/* Init data buffers */

	// Start monitor
	unified_monitor_start();

	for (int aux_i = 0; aux_i < 50; aux_i++)
	{
		/* text data FFEEDDCCBBAA99887766554433221100 */
		unsigned int j = 0x00000000;
		for (int i = 0; i < 16; ++i)
		{
			virtual_src_TEXT_addr[i] = j;
			j = j + 0x11;
		}

		j = 0x00000000;
		/* key data F0E0D0C0B0A09080706050403020100 */
		for (int i = 0; i < 32; ++i)
		{
			virtual_src_KEY_addr[i] = j;
			j = j + 0x1;
		}

		printf(" Data buffers initialized\n");

		memset(virtual_dst_ENCRYPTED_addr, 0, 16 * 4);

		/* Configure the accelerator */
		write_dma(aes_ip_virtual_addr, SLV_REG0, (OUT_SIZE << OUT_SIZE_SHIFT));

		/*  Reset the DMAs */
		write_dma(dma_TEXT_virtual_addr, MM2S_CONTROL_REGISTER, RESET_DMA);
		write_dma(dma_KEY_virtual_addr, MM2S_CONTROL_REGISTER, RESET_DMA);
		write_dma(dma_ENCRYPTED_virtual_addr, S2MM_CONTROL_REGISTER, RESET_DMA);

		/* Halt DMAs */
		write_dma(dma_TEXT_virtual_addr, MM2S_CONTROL_REGISTER, HALT_DMA);
		write_dma(dma_KEY_virtual_addr, MM2S_CONTROL_REGISTER, HALT_DMA);
		write_dma(dma_ENCRYPTED_virtual_addr, S2MM_CONTROL_REGISTER, HALT_DMA);

		/* Enable interrupts */
		write_dma(dma_TEXT_virtual_addr, MM2S_CONTROL_REGISTER, ENABLE_ALL_IRQ);
		write_dma(dma_KEY_virtual_addr, MM2S_CONTROL_REGISTER, ENABLE_ALL_IRQ);
		write_dma(dma_ENCRYPTED_virtual_addr, S2MM_CONTROL_REGISTER, ENABLE_ALL_IRQ);

		/*  Write the SOURCE/DESTINATION ADDRESS registers */
		write_dma(dma_TEXT_virtual_addr, MM2S_SRC_ADDRESS_REGISTER, TEXT_BUFFER);
		write_dma(dma_KEY_virtual_addr, MM2S_SRC_ADDRESS_REGISTER, KEY_BUFFER);
		write_dma(dma_ENCRYPTED_virtual_addr, S2MM_DST_ADDRESS_REGISTER, ENCRYPTED_BUFFER);

		/*  Run DMAs */
		write_dma(dma_KEY_virtual_addr, MM2S_CONTROL_REGISTER, RUN_DMA);
		write_dma(dma_TEXT_virtual_addr, MM2S_CONTROL_REGISTER, RUN_DMA);
		write_dma(dma_ENCRYPTED_virtual_addr, S2MM_CONTROL_REGISTER, RUN_DMA);

		/*  Write the LENGTH of the transfer
		*  Transfer will start right after this register is written
		*/

		printf(" DMA transfer started\n");

		write_dma(dma_TEXT_virtual_addr, MM2S_TRNSFR_LENGTH_REGISTER, TEXT_SIZE_BYTE);
		write_dma(dma_KEY_virtual_addr, MM2S_TRNSFR_LENGTH_REGISTER, KEY_SIZE_BYTE);
		write_dma(dma_ENCRYPTED_virtual_addr, S2MM_BUFF_LENGTH_REGISTER, ENCRYPTED_SIZE_BYTE);

		/* Wait for transfers completion */
		dma_mm2s_sync(dma_TEXT_virtual_addr);
		dma_mm2s_sync(dma_KEY_virtual_addr);
		dma_s2mm_sync(dma_ENCRYPTED_virtual_addr);
	}

	// Stop monitor
	unified_monitor_stop();

	printf("##########################################################################################\n");

	// unmapping
	munmap(dma_TEXT_virtual_addr, 65535);
	munmap(dma_KEY_virtual_addr, 65535);
	munmap(dma_ENCRYPTED_virtual_addr, 65535);
	munmap(aes_ip_virtual_addr, 65535);
	munmap(virtual_src_TEXT_addr, 65535);
	munmap(virtual_src_KEY_addr, 65535);
	munmap(virtual_dst_ENCRYPTED_addr, 65535);

	// close /dev/mem
	close(ddr_memory);

	// Read monitor
	unified_monitor_read();

	// Clean monitor
	unified_monitor_clean();

	return 0;
}
