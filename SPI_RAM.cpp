/*****************************************************************************
* | File        :   SPI_RAM.cpp
* | Author      :   Waveshare team
* | Function    :   23LC1024 (128 kB SPI SRAM) driver - frame buffer
* | Info        :
*   The frame buffer keeps one 16 bit pixel per address pair, exactly like the
*   V1 driver (address = (x + y * 160) * 2).  All transfers go out in
*   SPI_BLOCK_MAX sized blocks.
*----------------
* | This version:   V2.1
* | Date        :   2026-09-16
* | Info        :   C++ implementation of the V2 driver
*
******************************************************************************/
#include "pxt.h"
#include "LCD_Driver.h"     //shared SPI bus (lcdbus) + SPI settings
#include "SPI_RAM.h"

/*********************************************
function:
    Initialization system
*********************************************/
void SPIRAM::SPIRAM_SPI_Init(void)
{
    lcdbus::init();
}

/*******************************************************************************
function:
        // Mode handling
*******************************************************************************/
void SPIRAM::SPIRAM_Set_Mode(uint8_t mode)
{
    uint8_t cmd[2];

    cmd[0] = SRAM_CMD_WRSR;
    cmd[1] = mode;

    lcdbus::init();
    lcdbus::sync();         //finish an open frame buffer run first
    lcdbus::cs_ram(0);
    lcdbus::write(cmd, 2);
    lcdbus::cs_ram(1);
}

/*******************************************************************************
function:
        // Write and read byte
*******************************************************************************/
uint8_t SPIRAM::SPIRAM_RD_Byte(uint32_t Addr)
{
    uint8_t cmd[4];
    uint8_t Data;

    cmd[0] = SRAM_CMD_READ;
    cmd[1] = (uint8_t)(Addr >> 16);
    cmd[2] = (uint8_t)(Addr >> 8);
    cmd[3] = (uint8_t)(Addr);

    lcdbus::init();
    lcdbus::sync();
    lcdbus::cs_ram(0);
    lcdbus::write(cmd, 4);
    lcdbus::read(&Data, 1);
    lcdbus::cs_ram(1);

    return Data;
}

void SPIRAM::SPIRAM_WR_Byte(uint32_t Addr, uint8_t Data)
{
    uint8_t cmd[5];

    cmd[0] = SRAM_CMD_WRITE;
    cmd[1] = (uint8_t)(Addr >> 16);
    cmd[2] = (uint8_t)(Addr >> 8);
    cmd[3] = (uint8_t)(Addr);
    cmd[4] = Data;

    lcdbus::init();
    lcdbus::sync();
    lcdbus::cs_ram(0);
    lcdbus::write(cmd, 5);
    lcdbus::cs_ram(1);
}

/*******************************************************************************
function:
        // Write and read Len bytes.  Both run in sequential (stream) mode, the
        // address increments by itself, so one chip select phase can carry a
        // whole line.
*******************************************************************************/
void SPIRAM::SPIRAM_RD_Stream(uint32_t Addr, uint8_t *pBuf, uint32_t Len)
{
    uint8_t cmd[4];

    cmd[0] = SRAM_CMD_READ;
    cmd[1] = (uint8_t)(Addr >> 16);
    cmd[2] = (uint8_t)(Addr >> 8);
    cmd[3] = (uint8_t)(Addr);

    lcdbus::init();
    lcdbus::sync();
    lcdbus::cs_ram(0);
    lcdbus::write(cmd, 4);
    lcdbus::read(pBuf, Len);
    lcdbus::cs_ram(1);
}

void SPIRAM::SPIRAM_WR_Stream(uint32_t Addr, const uint8_t *pBuf, uint32_t Len)
{
    uint8_t cmd[4];

    cmd[0] = SRAM_CMD_WRITE;
    cmd[1] = (uint8_t)(Addr >> 16);
    cmd[2] = (uint8_t)(Addr >> 8);
    cmd[3] = (uint8_t)(Addr);

    lcdbus::init();
    lcdbus::sync();
    lcdbus::cs_ram(0);
    lcdbus::write(cmd, 4);
    lcdbus::write(pBuf, Len);
    lcdbus::cs_ram(1);
}

/*
 * Fill Len bytes with the same value: one block buffer is clocked out again
 * and again instead of sending the value byte by byte.
 */
void SPIRAM::SPIRAM_Fill(uint32_t Addr, uint32_t Len, uint8_t Data)
{
    uint8_t cmd[4];
    uint8_t block[SPI_BLOCK_MAX];
    uint32_t i;

    for (i = 0; i < SPI_BLOCK_MAX; i++)
        block[i] = Data;

    cmd[0] = SRAM_CMD_WRITE;
    cmd[1] = (uint8_t)(Addr >> 16);
    cmd[2] = (uint8_t)(Addr >> 8);
    cmd[3] = (uint8_t)(Addr);

    lcdbus::init();
    lcdbus::sync();
    lcdbus::cs_ram(0);
    lcdbus::write(cmd, 4);
    while (Len) {
        //every byte of the block holds Data, so a shorter final chunk can just
        //reuse the beginning of it
        uint32_t n = (Len > SPI_BLOCK_MAX) ? SPI_BLOCK_MAX : Len;
        lcdbus::write(block, n);
        Len -= n;
    }
    lcdbus::cs_ram(1);
}

/*
 * Fill with a repeating 16 bit pattern (one frame buffer pixel).  Both the
 * block and the transfer sizes are even, so the pattern stays aligned.
 */
void SPIRAM::SPIRAM_FillPattern(uint32_t Addr, uint32_t Len, uint16_t Data)
{
    uint8_t cmd[4];
    uint8_t block[SPI_BLOCK_MAX];
    uint32_t i;

    for (i = 0; i < SPI_BLOCK_MAX / 2; i++) {
        block[i * 2] = (uint8_t)(Data >> 8);
        block[i * 2 + 1] = (uint8_t)(Data & 0xFF);
    }

    cmd[0] = SRAM_CMD_WRITE;
    cmd[1] = (uint8_t)(Addr >> 16);
    cmd[2] = (uint8_t)(Addr >> 8);
    cmd[3] = (uint8_t)(Addr);

    lcdbus::init();
    lcdbus::sync();
    lcdbus::cs_ram(0);
    lcdbus::write(cmd, 4);
    while (Len) {
        uint32_t n = (Len > SPI_BLOCK_MAX) ? SPI_BLOCK_MAX : Len;
        lcdbus::write(block, n);
        Len -= n;
    }
    lcdbus::cs_ram(1);
}
