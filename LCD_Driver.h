/*****************************************************************************
* | File        :   LCD_Driver.h
* | Author      :   Waveshare team
* | Function    :   ST7735S (1.8 inch, 160x128) LCD driver
* | Info        :
*   Driver framework of the V1 release (native C++ SPI), adapted to
*   micro:bit V2 (CODAL).  Works on micro:bit V1 (mbed) as well.
*----------------
* | This version:   V2.1
* | Date        :   2026-09-16
* | Info        :   C++ implementation of the V2 driver
*
******************************************************************************/
#ifndef __LCD_DRIVER_H
#define __LCD_DRIVER_H

#include "pxt.h"

//Define the full screen height length of the display
#define LCD_WIDTH   160   //LCD width
#define LCD_HEIGHT  128   //LCD height

//SPI clock for the LCD and the SPI SRAM.
//The nRF52833 SPIM peripheral caps at 8 MHz (16/32 MHz only when SPIM3 is
//allocated), so 8 MHz is the fast, fully supported value.  Raise it if your
//module/wiring can take it.
#define LCD_SPI_FREQUENCY   8000000

//Biggest amount of data pushed in one SPI transfer.  The nRF52 EasyDMA window
//(and the CODAL NRF52SPI chunking limit) is 255 bytes, everything above that
//would fall back to a byte-per-byte (slow) transfer.
#define SPI_BLOCK_MAX       240

//16 bit RGB565 colours used by the driver itself
#define LCD_COLOR_WHITE     0xFFFF
#define LCD_COLOR_BLACK     0x0000

/*
 * The LCD and the SPI SRAM share one SPI bus (MOSI=P15, MISO=P14, SCK=P13),
 * each device has its own chip select - exactly like the V1 driver.  On
 * micro:bit V2 the CODAL compatibility header maps
 *     SPI               -> NRF52SPI
 *     MOSI/MISO/SCK     -> uBit.io.P15 / P14 / P13
 * on micro:bit V1 the mbed SPI class is used, so one implementation can drive
 * both boards.
 */
namespace lcdbus {
    void init();                                   //create + configure the bus (idempotent)
    void write(const uint8_t *buf, uint32_t len);  //send len bytes
    void read(uint8_t *buf, uint32_t len);         //clock in len bytes (MOSI kept low)
    void cs_lcd(int level);                        //LCD chip select
    void cs_ram(int level);                        //SPI SRAM chip select
}

class LCD_Driver {
private:
    void LCD_SPI_Init(void);
    void LCD_Reset(void);
    void LCD_InitReg(void);

    void LCD_WriteReg(uint8_t Reg);
    void LCD_WriteData_8Bit(uint8_t Data);
    void LCD_WriteData_Buf(uint16_t Buf, uint32_t Len);

    void LCD_SetWindows(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend);
    void LCD_SetCursor(uint16_t X, uint16_t Y);
    void LCD_SetColor(uint16_t Color, uint16_t Xnum, uint16_t Ynum);

    void LCD_SetPoint(uint16_t Xpoint, uint16_t Ypoint, uint16_t Color);

public:
    void LCD_Init(void);

    void LCD_SetBL(int Lev);

    void LCD_Clear(uint16_t Color);
    void LCD_ClearBuf(void);

    void LCD_Display(void);
    void LCD_DisplayWindows(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend);

    void LCD_DrawPoint(int Xpoint, int Ypoint, int Dot_Pixel, int Color);
    void LCD_DisChar_1207(int Xchar, int Ychar, int Char_Offset, int Color);
};

#endif
