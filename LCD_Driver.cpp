/*****************************************************************************
* | File        :   LCD_Driver.cpp
* | Author      :   Waveshare team
* | Function    :   ST7735S (1.8 inch, 160x128) LCD driver
* | Info        :
*   Driver framework of the V1 release (native C++ SPI), adapted to
*   micro:bit V2 (CODAL).  Every byte of SPI traffic is generated in native
*   code, therefore a full screen refresh costs the raw SPI time instead of
*   tens of thousands of pins.spiWrite() round trips through the interpreter.
*----------------
* | This version:   V2.1
* | Date        :   2026-09-16
* | Info        :   C++ implementation of the V2 driver
*
******************************************************************************/
#include "pxt.h"
#include "MicroBit.h"
#include "LCD_Driver.h"
#include "SPI_RAM.h"
#include "Font12.h"

// ---------------------------------------------------------------------------
//  Pin usage (same wiring as the V1 module / the V2 TypeScript driver)
//      LCD : RESET = P8, DC = P12, CS = P16, back light = P1
//      RAM : CS = P2
//      SPI : MOSI = P15, MISO = P14, SCK = P13
// ---------------------------------------------------------------------------
#define LCD_PIN_RST     MICROBIT_ID_IO_P8
#define LCD_PIN_DC      MICROBIT_ID_IO_P12
#define LCD_PIN_CS      MICROBIT_ID_IO_P16
#define LCD_PIN_BL      MICROBIT_ID_IO_P1
#define RAM_PIN_CS      MICROBIT_ID_IO_P2

#define LCD_RST_WRITE(v)    pxt::getPin(LCD_PIN_RST)->setDigitalValue(v)
#define LCD_DC_WRITE(v)     pxt::getPin(LCD_PIN_DC)->setDigitalValue(v)
#define LCD_CS_WRITE(v)     pxt::getPin(LCD_PIN_CS)->setDigitalValue(v)
#define RAM_CS_WRITE(v)     pxt::getPin(RAM_PIN_CS)->setDigitalValue(v)

//delay
//On micro:bit V2 the driver runs inside a fiber, so fiber_sleep() keeps the
//scheduler (radio, buttons, timers) alive while waiting.
#if MICROBIT_CODAL
#define Driver_Delay_ms(xms)    codal::fiber_sleep((unsigned long)(xms))
#else
#define Driver_Delay_ms(xms)    wait_ms(xms)
#endif

// ---------------------------------------------------------------------------
//  Shared SPI bus
// ---------------------------------------------------------------------------
#if MICROBIT_CODAL
typedef codal::NRF52SPI SpiBus;     //micro:bit V2 (CODAL)
#else
typedef SPI SpiBus;                 //micro:bit V1 (mbed)
#endif

//The bus is created on first use (LCD_Init), i.e. after the runtime has set
//up uBit: constructing it as a global would depend on the static
//initialisation order of the pxt/CODAL objects.
static SpiBus *lcd_spi = NULL;

//EasyDMA (micro:bit V2) can only read its buffers from RAM, never from flash,
//so the "send zeros while reading" buffer has to live in RAM.
static uint8_t spi_dummy_tx[SPI_BLOCK_MAX] = { 0 };

void lcdbus::init()
{
    if (lcd_spi != NULL)
        return;

    lcd_spi = new SpiBus(MOSI, MISO, SCK);
    lcd_spi->format(8, 0);
    lcd_spi->frequency(LCD_SPI_FREQUENCY);
}

void lcdbus::write(const uint8_t *buf, uint32_t len)
{
    if (lcd_spi == NULL)
        init();

    while (len) {
        uint32_t n = (len > SPI_BLOCK_MAX) ? SPI_BLOCK_MAX : len;
#if MICROBIT_CODAL
        lcd_spi->transfer(buf, n, NULL, 0);
#else
        lcd_spi->write((const char *)buf, (int)n, NULL, 0);
#endif
        buf += n;
        len -= n;
    }
}

void lcdbus::read(uint8_t *buf, uint32_t len)
{
    if (lcd_spi == NULL)
        init();

    while (len) {
        uint32_t n = (len > SPI_BLOCK_MAX) ? SPI_BLOCK_MAX : len;
#if MICROBIT_CODAL
        lcd_spi->transfer(spi_dummy_tx, n, buf, n);
#else
        lcd_spi->write((const char *)spi_dummy_tx, (int)n, (char *)buf, (int)n);
#endif
        buf += n;
        len -= n;
    }
}

void lcdbus::cs_lcd(int level)
{
    LCD_CS_WRITE(level);
}

void lcdbus::cs_ram(int level)
{
    RAM_CS_WRITE(level);
}

//single byte transfer (register/command access)
static void lcd_spi_write_byte(uint8_t data)
{
    if (lcd_spi == NULL)
        lcdbus::init();

    lcd_spi->write((int)data);
}

//The frame buffer lives in the on board SPI SRAM (23LC1024)
static SPIRAM spiram;

/*********************************************
function:
    Initialization system
*********************************************/
void LCD_Driver::LCD_SPI_Init()
{
    lcdbus::init();
}

/*******************************************************************************
function:
    Hardware reset
*******************************************************************************/
void LCD_Driver::LCD_Reset()
{
    LCD_RST_WRITE(1);
    Driver_Delay_ms(100);
    LCD_RST_WRITE(0);
    Driver_Delay_ms(100);
    LCD_RST_WRITE(1);
    Driver_Delay_ms(100);
}

/*******************************************************************************
function:
    Write register address and data
*******************************************************************************/
void LCD_Driver::LCD_WriteReg(uint8_t Reg)
{
    LCD_DC_WRITE(0);
    LCD_CS_WRITE(0);
    lcd_spi_write_byte(Reg);
    LCD_CS_WRITE(1);
}

void LCD_Driver::LCD_WriteData_8Bit(uint8_t Data)
{
    LCD_DC_WRITE(1);
    LCD_CS_WRITE(0);
    lcd_spi_write_byte(Data);
    LCD_CS_WRITE(1);
}

/*
 * Send the same 16 bit colour Len times, in SPI_BLOCK_MAX sized blocks: the V1
 * driver clocked out both bytes of every pixel individually.
 */
void LCD_Driver::LCD_WriteData_Buf(uint16_t Buf, uint32_t Len)
{
    uint8_t block[SPI_BLOCK_MAX];
    uint32_t i;
    uint32_t bytes = Len * 2;

    for (i = 0; i < SPI_BLOCK_MAX / 2; i++) {
        block[i * 2] = (uint8_t)(Buf >> 8);
        block[i * 2 + 1] = (uint8_t)(Buf & 0xFF);
    }

    LCD_DC_WRITE(1);
    LCD_CS_WRITE(0);
    while (bytes) {
        //SPI_BLOCK_MAX is even, so a chunk never splits a pixel in half
        uint32_t n = (bytes > SPI_BLOCK_MAX) ? SPI_BLOCK_MAX : bytes;
        lcdbus::write(block, n);
        bytes -= n;
    }
    LCD_CS_WRITE(1);
}

/*******************************************************************************
function:
    Common register initialization
*******************************************************************************/
void LCD_Driver::LCD_InitReg()
{
    //ST7735R Frame Rate
    LCD_WriteReg(0xB1);
    LCD_WriteData_8Bit(0x01);
    LCD_WriteData_8Bit(0x2C);
    LCD_WriteData_8Bit(0x2D);

    LCD_WriteReg(0xB2);
    LCD_WriteData_8Bit(0x01);
    LCD_WriteData_8Bit(0x2C);
    LCD_WriteData_8Bit(0x2D);

    LCD_WriteReg(0xB3);
    LCD_WriteData_8Bit(0x01);
    LCD_WriteData_8Bit(0x2C);
    LCD_WriteData_8Bit(0x2D);
    LCD_WriteData_8Bit(0x01);
    LCD_WriteData_8Bit(0x2C);
    LCD_WriteData_8Bit(0x2D);

    LCD_WriteReg(0xB4); //Column inversion
    LCD_WriteData_8Bit(0x07);

    //ST7735R Power Sequence
    LCD_WriteReg(0xC0);
    LCD_WriteData_8Bit(0xA2);
    LCD_WriteData_8Bit(0x02);
    LCD_WriteData_8Bit(0x84);
    LCD_WriteReg(0xC1);
    LCD_WriteData_8Bit(0xC5);

    LCD_WriteReg(0xC2);
    LCD_WriteData_8Bit(0x0A);
    LCD_WriteData_8Bit(0x00);

    LCD_WriteReg(0xC3);
    LCD_WriteData_8Bit(0x8A);
    LCD_WriteData_8Bit(0x2A);
    LCD_WriteReg(0xC4);
    LCD_WriteData_8Bit(0x8A);
    LCD_WriteData_8Bit(0xEE);

    LCD_WriteReg(0xC5); //VCOM
    LCD_WriteData_8Bit(0x0E);

    //ST7735R Gamma Sequence
    LCD_WriteReg(0xe0);
    LCD_WriteData_8Bit(0x0f);
    LCD_WriteData_8Bit(0x1a);
    LCD_WriteData_8Bit(0x0f);
    LCD_WriteData_8Bit(0x18);
    LCD_WriteData_8Bit(0x2f);
    LCD_WriteData_8Bit(0x28);
    LCD_WriteData_8Bit(0x20);
    LCD_WriteData_8Bit(0x22);
    LCD_WriteData_8Bit(0x1f);
    LCD_WriteData_8Bit(0x1b);
    LCD_WriteData_8Bit(0x23);
    LCD_WriteData_8Bit(0x37);
    LCD_WriteData_8Bit(0x00);
    LCD_WriteData_8Bit(0x07);
    LCD_WriteData_8Bit(0x02);
    LCD_WriteData_8Bit(0x10);

    LCD_WriteReg(0xe1);
    LCD_WriteData_8Bit(0x0f);
    LCD_WriteData_8Bit(0x1b);
    LCD_WriteData_8Bit(0x0f);
    LCD_WriteData_8Bit(0x17);
    LCD_WriteData_8Bit(0x33);
    LCD_WriteData_8Bit(0x2c);
    LCD_WriteData_8Bit(0x29);
    LCD_WriteData_8Bit(0x2e);
    LCD_WriteData_8Bit(0x30);
    LCD_WriteData_8Bit(0x30);
    LCD_WriteData_8Bit(0x39);
    LCD_WriteData_8Bit(0x3f);
    LCD_WriteData_8Bit(0x00);
    LCD_WriteData_8Bit(0x07);
    LCD_WriteData_8Bit(0x03);
    LCD_WriteData_8Bit(0x10);

    LCD_WriteReg(0xF0); //Enable test command
    LCD_WriteData_8Bit(0x01);

    LCD_WriteReg(0xF6); //Disable ram power save mode
    LCD_WriteData_8Bit(0x00);

    LCD_WriteReg(0x3A); //65k mode
    LCD_WriteData_8Bit(0x05);

    LCD_WriteReg(0x36); //MX, MY, RGB mode
    LCD_WriteData_8Bit(0xF7 & 0xA0); //RGB color filter panel
}

/********************************************************************************
function:   Sets the start position and size of the display area
parameter:
    Xstart  :   X direction Start coordinates
    Ystart  :   Y direction Start coordinates
    Xend    :   X direction end coordinates
    Yend    :   Y direction end coordinates
********************************************************************************/
void LCD_Driver::LCD_SetWindows(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend)
{
    //set the X coordinates
    LCD_WriteReg(0x2A);
    LCD_WriteData_8Bit(0x00);
    LCD_WriteData_8Bit((Xstart & 0xff) + 1);
    LCD_WriteData_8Bit(0x00);
    LCD_WriteData_8Bit(((Xend - 1) & 0xff) + 1);

    //set the Y coordinates
    LCD_WriteReg(0x2B);
    LCD_WriteData_8Bit(0x00);
    LCD_WriteData_8Bit((Ystart & 0xff) + 2);
    LCD_WriteData_8Bit(0x00);
    LCD_WriteData_8Bit(((Yend - 1) & 0xff) + 2);

    LCD_WriteReg(0x2C);
}

/********************************************************************************
function:
    Set the display point (Xpoint, Ypoint)
********************************************************************************/
void LCD_Driver::LCD_SetCursor(uint16_t Xpoint, uint16_t Ypoint)
{
    LCD_SetWindows(Xpoint, Ypoint, Xpoint, Ypoint);
}

/********************************************************************************
function:
    Set show color
********************************************************************************/
void LCD_Driver::LCD_SetColor(uint16_t Color, uint16_t Xnum, uint16_t Ynum)
{
    LCD_WriteData_Buf(Color, (uint32_t)Xnum * (uint32_t)Ynum);
}

/********************************************************************************
function:
    initialization
********************************************************************************/
void LCD_Driver::LCD_Init()
{
    LCD_SPI_Init();

    //chip selects high (both slaves idle) before anything is clocked out
    LCD_CS_WRITE(1);
    RAM_CS_WRITE(1);

    spiram.SPIRAM_SPI_Init();
    //Stream (sequential) mode: the address auto increments after every byte,
    //so command + address + data bytes can share one chip select phase.  The
    //pixel write below sends two data bytes per transaction, which only works
    //in stream mode - the V1 driver needed a separate transaction per byte.
    spiram.SPIRAM_Set_Mode(SRAM_STREAM_MODE);

    //back light
    LCD_SetBL(1023);

    //Hardware reset
    LCD_Reset();

    //Set the initialization register
    LCD_InitReg();

    //sleep out
    LCD_WriteReg(0x11);
    Driver_Delay_ms(120);

    //Turn on the LCD display
    LCD_WriteReg(0x29);
}

/********************************************************************************
function:
    Back light brightness.
    The V1 release used a 0..330 range (duty = Lev / 100, clamped to 0..1) and
    the V2 block offers 0..1023: everything from 330 upwards is full
    brightness, so V1 and V2 programs both behave as expected.
********************************************************************************/
void LCD_Driver::LCD_SetBL(int Lev)
{
    int Pwm;

    if (Lev < 0)
        Lev = 0;
    if (Lev > 330)
        Lev = 330;
    Pwm = Lev * 1023 / 330;

    pxt::getPin(LCD_PIN_BL)->setAnalogValue(Pwm);
}

/********************************************************************************
function:
    Clear screen
********************************************************************************/
void LCD_Driver::LCD_Clear(uint16_t Color)
{
    LCD_SetWindows(0, 0, LCD_WIDTH, LCD_HEIGHT);
    LCD_SetColor(Color, LCD_WIDTH + 2, LCD_HEIGHT + 2);
}

/********************************************************************************
function:
    Clear the frame buffer in the SPI SRAM
********************************************************************************/
void LCD_Driver::LCD_ClearBuf()
{
    lcdbus::init();
    spiram.SPIRAM_Set_Mode(SRAM_STREAM_MODE);
    spiram.SPIRAM_Fill(0, (uint32_t)LCD_WIDTH * LCD_HEIGHT * 2, 0xFF);
}

void LCD_Driver::LCD_SetPoint(uint16_t Xpoint, uint16_t Ypoint, uint16_t Color)
{
    uint32_t Addr = ((uint32_t)Xpoint + (uint32_t)Ypoint * LCD_WIDTH) * 2;

    spiram.SPIRAM_WR_Word(Addr, Color);
}

/********************************************************************************
function:
    Show the whole frame buffer on the LCD
********************************************************************************/
void LCD_Driver::LCD_Display()
{
    uint8_t RBUF[LCD_WIDTH * 2];    //read one line (320 bytes) at a time
    uint16_t y;

    lcdbus::init();
    spiram.SPIRAM_Set_Mode(SRAM_STREAM_MODE);
    LCD_SetWindows(0, 0, LCD_WIDTH, LCD_HEIGHT);
    for (y = 0; y < LCD_HEIGHT; y++) {
        spiram.SPIRAM_RD_Stream((uint32_t)y * LCD_WIDTH * 2, RBUF, LCD_WIDTH * 2);

        LCD_DC_WRITE(1);
        LCD_CS_WRITE(0);
        lcdbus::write(RBUF, LCD_WIDTH * 2);
        LCD_CS_WRITE(1);
    }

    //Turn on the LCD display
    LCD_WriteReg(0x29);
}

/********************************************************************************
function:
    Show a window of the frame buffer
parameter:
    Xstart, Ystart : top left corner (1 based, like the blocks)
    Xend, Yend     : bottom right corner
********************************************************************************/
void LCD_Driver::LCD_DisplayWindows(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend)
{
    uint8_t RBUF[LCD_WIDTH * 2];
    uint16_t y;
    uint32_t pixels;

    if (Xend <= Xstart || Yend <= Ystart)
        return;
    if (Xend > LCD_WIDTH)
        Xend = LCD_WIDTH;
    if (Yend > LCD_HEIGHT)
        Yend = LCD_HEIGHT;

    pixels = Xend - Xstart;     //pixels per line

    lcdbus::init();
    spiram.SPIRAM_Set_Mode(SRAM_STREAM_MODE);
    LCD_SetWindows(Xstart, Ystart, Xend, Yend);
    for (y = Ystart; y < Yend; y++) {
        spiram.SPIRAM_RD_Stream(((uint32_t)y * LCD_WIDTH + Xstart) * 2, RBUF, pixels * 2);

        LCD_DC_WRITE(1);
        LCD_CS_WRITE(0);
        lcdbus::write(RBUF, pixels * 2);
        LCD_CS_WRITE(1);
    }
}

/********************************************************************************
function:
    Draw a point into the frame buffer
********************************************************************************/
void LCD_Driver::LCD_DrawPoint(int Xpoint, int Ypoint, int Dot_Pixel, int Color)
{
    int XDir_Num, YDir_Num;

    lcdbus::init();
    for (XDir_Num = 0; XDir_Num < Dot_Pixel; XDir_Num++) {
        for (YDir_Num = 0; YDir_Num < Dot_Pixel; YDir_Num++) {
            LCD_SetPoint(Xpoint + XDir_Num - Dot_Pixel, Ypoint + YDir_Num - Dot_Pixel, Color);
        }
    }
}

/********************************************************************************
function:
    Draw one 12x7 character into the frame buffer
********************************************************************************/
void LCD_Driver::LCD_DisChar_1207(int Xchar, int Ychar, int Char_Offset, int Color)
{
    int Page = 0, Column = 0;
    const unsigned char *ptr = &Font12_Table[Char_Offset];

    lcdbus::init();
    for (Page = 0; Page < 12; Page++) {
        for (Column = 0; Column < 7; Column++) {
            if (*ptr & (0x80 >> (Column % 8)))
                LCD_SetPoint(Xchar + Column, Ychar + Page, Color);

            //One pixel is 8 bits
            if (Column % 8 == 7)
                ptr++;
        }// Write a line
        if (7 % 8 != 0)
            ptr++;
    }// Write all
}
