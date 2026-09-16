/*****************************************************************************
* | File        :   SPI_RAM.h
* | Author      :   Waveshare team
* | Function    :   23LC1024 (128 kB SPI SRAM) driver - frame buffer
* | Info        :
*   Same API as the V1 release, adapted to micro:bit V2 (CODAL).
*----------------
* | This version:   V2.1
* | Date        :   2026-09-16
* | Info        :   C++ implementation of the V2 driver
*
******************************************************************************/
#ifndef __SPI_RAM_H_
#define __SPI_RAM_H_

#include "pxt.h"

// SRAM opcodes
#define SRAM_CMD_WREN   0x06
#define SRAM_CMD_WRDI   0x04
#define SRAM_CMD_RDSR   0x05
#define SRAM_CMD_WRSR   0x01
#define SRAM_CMD_READ   0x03
#define SRAM_CMD_WRITE  0x02

// SRAM modes
#define SRAM_BYTE_MODE      0x00
#define SRAM_PAGE_MODE      0x80
#define SRAM_STREAM_MODE    0x40

class SPIRAM {
public:
    void SPIRAM_SPI_Init(void);

    void SPIRAM_Set_Mode(uint8_t mode);

    uint8_t SPIRAM_RD_Byte(uint32_t Addr);
    void SPIRAM_WR_Byte(uint32_t Addr, uint8_t Data);

    void SPIRAM_RD_Stream(uint32_t Addr, uint8_t *pBuf, uint32_t Len);
    void SPIRAM_WR_Stream(uint32_t Addr, const uint8_t *pBuf, uint32_t Len);
    void SPIRAM_Fill(uint32_t Addr, uint32_t Len, uint8_t Data);
    void SPIRAM_FillPattern(uint32_t Addr, uint32_t Len, uint16_t Data);
};

#endif
