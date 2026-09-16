/*****************************************************************************
* | File        :   SPI_RAM.cpp
* | Author      :   Waveshare team
* | Function    :   23LC1024 (128 kB SPI SRAM) driver - frame buffer
* | Info        :
*   The frame buffer keeps one 16 bit pixel per address pair, exactly like the
*   V1 driver (address = (x + y * 160) * 2).
*----------------
* | This version:   V2.1
* | Date        :   2026-09-16
* | Info        :   C++ implementation of the V2 driver
*
******************************************************************************/
#include "pxt.h"
#include "LCD_Driver.h"     //shared SPI bus (lcdbus) + LCD_SPI settings
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
    uint8_t cmd[5];
    uint8_t Data;

    cmd[0] = SRAM_CMD_READ;
    cmd[1] = (uint8_t)(Addr >> 16);
    cmd[2] = (uint8_t)(Addr >> 8);
    cmd[3] = (uint8_t)(Addr);
    cmd[4] = 0x00;

    lcdbus::init();
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
    lcdbus::cs_ram(0);
    lcdbus::write(cmd, 5);
    lcdbus::cs_ram(1);
}

/*
 * One 16 bit pixel (high byte first, like the V1 driver) in a single chip
 * select phase.  The V1 driver needed two SPIRAM_WR_Byte() calls (and each of
 * them clocked out 5 bytes one by one) for one pixel.
 */
void SPIRAM::SPIRAM_WR_Word(uint32_t Addr, uint16_t Data)
{
    uint8_t cmd[6];

    cmd[0] = SRAM_CMD_WRITE;
    cmd[1] = (uint8_t)(Addr >> 16);
    cmd[2] = (uint8_t)(Addr >> 8);
    cmd[3] = (uint8_t)(Addr);
    cmd[4] = (uint8_t)(Data >> 8);
    cmd[5] = (uint8_t)(Data);

    lcdbus::init();
    lcdbus::cs_ram(0);
    lcdbus::write(cmd, 6);
    lcdbus::cs_ram(1);
}

/*******************************************************************************
function:
        // Page transfer functions. Bound to current page. Passing the boundary
        //  will wrap to the beginning
*******************************************************************************/
void SPIRAM::SPIRAM_RD_Page(uint32_t Addr, uint8_t *pBuf)
{
    SPIRAM_RD_Stream(Addr, pBuf, 32);
}

void SPIRAM::SPIRAM_WR_Page(uint32_t Addr, uint8_t *pBuf)
{
    SPIRAM_WR_Stream(Addr, pBuf, 32);
}

/*******************************************************************************
function:
        // Write and read Len
*******************************************************************************/
void SPIRAM::SPIRAM_RD_Stream(uint32_t Addr, uint8_t *pBuf, uint32_t Len)
{
    uint8_t cmd[4];

    cmd[0] = SRAM_CMD_READ;
    cmd[1] = (uint8_t)(Addr >> 16);
    cmd[2] = (uint8_t)(Addr >> 8);
    cmd[3] = (uint8_t)(Addr);

    lcdbus::init();
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
    lcdbus::cs_ram(0);
    lcdbus::write(cmd, 4);
    lcdbus::write(pBuf, Len);
    lcdbus::cs_ram(1);
}

/*
 * Fill Len bytes with the same value, used to wipe the frame buffer (the V1
 * driver clocked out 40960 bytes one by one from TypeScript).
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
