#ifndef DMA_SIMPLEMODE_H
#define DMA_SIMPLEMODE_H

// OFFSET DMAs
#define OFFS_DMA_TEXT_DATA 0x00A0020000
#define OFFS_DMA_KEY_DATA 0x00A0030000
#define OFFS_DMA_ENCRYPT_DATA 0x00A0040000

// offsets specified in AXI DMA reference guide
// https://www.xilinx.com/support/documentation/ip_documentation/axi_dma/v7_1/pg021_axi_dma.pdf
#define MM2S_CONTROL_REGISTER 0x00
#define MM2S_STATUS_REGISTER 0x04
#define MM2S_SRC_ADDRESS_REGISTER 0x18
#define MM2S_TRNSFR_LENGTH_REGISTER 0x28

#define S2MM_CONTROL_REGISTER 0x30
#define S2MM_STATUS_REGISTER 0x34
#define S2MM_DST_ADDRESS_REGISTER 0x48
#define S2MM_BUFF_LENGTH_REGISTER 0x58

// FLAGS for synchronization
#define IOC_IRQ_FLAG 1 << 12 // 0001 0000 0000 0000 = 1000
#define IDLE_FLAG 1 << 1	 // 0000 0000 0000 0010 = 0002

// check status content
#define STATUS_HALTED 0x00000001		   // 0000 0000 0000 0000 . 0000 0000 0000 0001
#define STATUS_IDLE 0x00000002			   // 0000 0000 0000 0000 . 0000 0000 0000 0010
#define STATUS_SG_INCLDED 0x00000008	   // 0000 0000 0000 0000 . 0000 0000 0000 1000
#define STATUS_DMA_INTERNAL_ERR 0x00000010 // 0000 0000 0000 0000 . 0000 0000 0001 0000
#define STATUS_DMA_SLAVE_ERR 0x00000020	   // 0000 0000 0000 0000 . 0000 0000 0010 0000
#define STATUS_DMA_DECODE_ERR 0x00000040   // 0000 0000 0000 0000 . 0000 0000 0100 0000
#define STATUS_SG_INTERNAL_ERR 0x00000100  // 0000 0000 0000 0000 . 0000 0001 0000 0000
#define STATUS_SG_SLAVE_ERR 0x00000200	   // 0000 0000 0000 0000 . 0000 0010 0000 0000
#define STATUS_SG_DECODE_ERR 0x00000400	   // 0000 0000 0000 0000 . 0000 0100 0000 0000
#define STATUS_IOC_IRQ 0x00001000		   // 0000 0000 0000 0000 . 0001 0000 0000 0000
#define STATUS_DELAY_IRQ 0x00002000		   // 0000 0000 0000 0000 . 0010 0000 0000 0000
#define STATUS_ERR_IRQ 0x00004000		   // 0000 0000 0000 0000 . 0100 0000 0000 0000

// control
#define HALT_DMA 0x00000000			// 0000 0000 0000 0000 . 0000 0000 0000 0000
#define RUN_DMA 0x00000001			// 0000 0000 0000 0000 . 0000 0000 0000 0001
#define RESET_DMA 0x00000004		// 0000 0000 0000 0000 . 0000 0000 0000 0100
#define ENABLE_IOC_IRQ 0x00001000	// 0000 0000 0000 0000 . 0001 0000 0000 0000
#define ENABLE_DELAY_IRQ 0x00002000 // 0000 0000 0000 0000 . 0010 0000 0000 0000
#define ENABLE_ERR_IRQ 0x00004000	// 0000 0000 0000 0000 . 0100 0000 0000 0000
#define ENABLE_ALL_IRQ 0x00007000	// 0000 0000 0000 0000 . 0111 0000 0000 0000


unsigned int write_dma(unsigned int *virtual_addr, int offset, unsigned int value);


unsigned int read_dma(unsigned int *virtual_addr, int offset);


void dma_s2mm_status(unsigned int *virtual_addr);


void dma_mm2s_status(unsigned int *virtual_addr);


int dma_mm2s_sync(unsigned int *virtual_addr);

int dma_s2mm_sync(unsigned int *virtual_addr);

void print_mem(void *virtual_address, int byte_count);


#endif //DMA_SIMPLEMODE_H
