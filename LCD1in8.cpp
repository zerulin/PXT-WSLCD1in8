/*****************************************************************************
* | File        :   LCD1in8.cpp
* | Author      :   Waveshare team
* | Function    :   Native (shim) entry points of the LCD1in8 blocks
* | Info        :
*   The blocks themselves are declared in main.ts, every heavy function is
*   forwarded to the C++ driver here (V1 framework, adapted to micro:bit V2).
*----------------
* | This version:   V2.1
* | Date        :   2026-09-16
* | Info        :   C++ implementation of the V2 driver
*
******************************************************************************/
#include "pxt.h"
#include "LCD_Driver.h"

using namespace pxt;

static LCD_Driver lcd;

namespace LCD1IN8 {

    //%
    void LCD_Init() {
        lcd.LCD_Init();
    }

    //%
    void LCD_Clear() {
        lcd.LCD_Clear(LCD_COLOR_WHITE);
        lcd.LCD_ClearBuf();
    }

    //%
    void LCD_Filling(int Color) {
        lcd.LCD_Clear(Color);
    }

    //%
    void LCD_ClearBuf() {
        lcd.LCD_ClearBuf();
    }

    //%
    void LCD_Display() {
        lcd.LCD_Display();
    }

    //%
    void LCD_DisplayWindows(int Xstart, int Ystart, int Xend, int Yend) {
        lcd.LCD_DisplayWindows(Xstart, Ystart, Xend, Yend);
    }

    //%
    void LCD_SetBL(int Lev) {
        lcd.LCD_SetBL(Lev);
    }

    //%
    void DrawPoint(int x, int y, int Color, int Dot) {
        lcd.LCD_DrawPoint(x, y, Dot, Color);
    }

    //%
    void DisChar_1207(int Xchar, int Ychar, int Char_Offset, int Color) {
        lcd.LCD_DisChar_1207(Xchar, Ychar, Char_Offset, Color);
    }
}
