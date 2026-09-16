/*****************************************************************************
* | File        :   LCD1in8.cpp
* | Author      :   Waveshare team
* | Function    :   Native (shim) entry points of the LCD1in8 blocks
* | Info        :
*   The blocks are declared in main.ts, every one of them is forwarded to the
*   C++ driver here (V1 framework, adapted to micro:bit V2).  One shim call
*   per block / per character, so the interpreter never walks pixels again.
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
    void DrawLine(int Xstart, int Ystart, int Xend, int Yend, int Color, int Line_width, int Line_Style) {
        lcd.LCD_DrawLine(Xstart, Ystart, Xend, Yend, Color, Line_width, Line_Style);
    }

    //%
    void DrawRectangle(int Xstart, int Ystart, int Xend, int Yend, int Color, int Filled, int Dot_Pixel) {
        lcd.LCD_DrawRectangle(Xstart, Ystart, Xend, Yend, Color, Filled, Dot_Pixel);
    }

    //%
    void DrawCircle(int X_Center, int Y_Center, int Radius, int Color, int Draw_Fill, int Dot_Pixel) {
        lcd.LCD_DrawCircle(X_Center, Y_Center, Radius, Color, Draw_Fill, Dot_Pixel);
    }

    //%
    void DisChar_1207(int Xchar, int Ychar, int Char_Offset, int Color) {
        lcd.LCD_DisChar_1207(Xchar, Ychar, Char_Offset, Color);
    }
}
