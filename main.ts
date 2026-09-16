/*****************************************************************************
* | File        :   1in8LCD.ts
* | Author      :   hnwangkg-ezio for Waveshare 
* | Function    :   Contorl 1.8inch lcd Show
* | Info        :
*----------------
* | This version:   V2.1
* | Date        :   2026-09-16
* | Info        :   for micro:bit v2
*                   The blocks are unchanged, but all hardware access and all
*                   drawing loops now run in the native driver (LCD1in8.cpp +
*                   LCD_Driver.cpp + SPI_RAM.cpp) like the V1 release did.
*                   Pushing every byte, and every pixel, through the
*                   interpreter (pins.spiWrite) was what made the graphics
*                   refresh slow.  LCD/RAM geometry lives in LCD_Driver.h, the
*                   font table in Font12.h.
*
******************************************************************************/
enum COLOR {
    WHITE = 0xFFFF,
    BLACK = 0x0000,
    BLUE = 0x001F,
    BRED = 0XF81F,
    GRED = 0XFFE0,
    GBLUE = 0X07FF,
    RED = 0xF800,
    MAGENTA = 0xF81F,
    GREEN = 0x07E0,
    CYAN = 0x7FFF,
    YELLOW = 0xFFE0,
    BROWN = 0XBC40,
    BRRED = 0XFC07,
    GRAY = 0X8430
}

enum DOT_PIXEL{
    DOT_PIXEL_1 = 1,
    DOT_PIXEL_2,
    DOT_PIXEL_3,
    DOT_PIXEL_4
};

enum LINE_STYLE {
    LINE_SOLID = 0,
    LINE_DOTTED,
};

enum DRAW_FILL {
    DRAW_EMPTY = 0,
    DRAW_FULL,
};

//% weight=20 color=#436EEE icon="\uf108"
namespace LCD1IN8 {
    //% blockId=LCD_Init
    //% blockGap=8
    //% block="LCD1IN8 Init"
    //% shim=LCD1IN8::LCD_Init
    //% weight=200
    export function LCD_Init(): void{
    }

    //% blockId=LCD_Clear
    //% blockGap=8
    //% block="LCD Clear"
    //% shim=LCD1IN8::LCD_Clear
    //% weight=195
    export function LCD_Clear(): void{
    }

    //% blockId=LCD_Filling
    //% blockGap=8
    //% block="Filling Color %Color"
    //% shim=LCD1IN8::LCD_Filling
    //% weight=195
    export function LCD_Filling(Color: COLOR): void{
    }

    //% blockId=Draw_Clear
    //% blockGap=8
    //% block="Clear Drawing cache"
    //% shim=LCD1IN8::LCD_ClearBuf
    //% weight=195
    export function LCD_ClearBuf(): void {
    }

    //% blockId=LCD_Display
    //% blockGap=8
    //% block="Show Full Screen"
    //% shim=LCD1IN8::LCD_Display
    //% weight=190
    export function LCD_Display(): void {
    }

    //% blockId=LCD_DisplayWindows
    //% blockGap=8
    //% block="Show Windows|Xstart %Xstart|Ystart %Ystart|Xend %Xend|Yend %Yend"
    //% shim=LCD1IN8::LCD_DisplayWindows
    //% Xstart.min=1 Xstart.max=160 Ystart.min=1 Ystart.max=128
    //% Xend.min=1 Xend.max=160 Yend.min=1 Yend.max=128
    //% weight=189
    export function LCD_DisplayWindows(Xstart: number, Ystart: number, Xend: number, Yend: number): void {
    }

    //% blockId=LCD_SetBL
    //% blockGap=8
    //% block="Set back light level %Lev"
    //% Lev.min=0 Lev.max=1023
    //% shim=LCD1IN8::LCD_SetBL
    //% weight=180
    export function LCD_SetBL(Lev: number): void{
    }

    //% blockId=DrawPoint
    //% blockGap=8
    //% block="Draw Point|x %Xpoint|y %Ypoint|Color %Color|Point Size %Dot_Pixel"
    //% Xpoint.min=1 Xpoint.max=160 Ypoint.min=1 Ypoint.max=128
    //% Color.min=0 Color.max=65535
    //% shim=LCD1IN8::DrawPoint
    //% weight=150
    export function DrawPoint(Xpoint:number, Ypoint:number, Color:number, Dot_Pixel:DOT_PIXEL): void {
    }

    //% blockId=DrawLine
    //% blockGap=8
    //% block="Draw Line|Xstart %Xstart|Ystart %Ystart|Xend %Xend|Yend %Yend|Color %Color|width %Line_width|Style %Line_Style"
    //% Xstart.min=1 Xstart.max=160 Ystart.min=1 Ystart.max=128
    //% Xend.min=1 Xend.max=160 Yend.min=1 Yend.max=128
    //% Color.min=0 Color.max=65535
    //% shim=LCD1IN8::DrawLine
    //% weight=140
    export function DrawLine(Xstart: number, Ystart: number, Xend: number, Yend: number, Color: number, Line_width: DOT_PIXEL, Line_Style: LINE_STYLE): void {
    }

    //% blockId=DrawRectangle
    //% blockGap=8
    //% block="Draw Rectangle|Xstart2 %Xstart2|Ystart2 %Ystart2|Xend2 %Xend2|Yend2 %Yend2|Color %Color|Filled %Filled |Line width %Dot_Pixel"
    //% Xstart2.min=1 Xstart2.max=160 Ystart2.min=1 Ystart2.max=128 
    //% Xend2.min=1 Xend2.max=160 Yend2.min=1 Yend2.max=128
    //% Color.min=0 Color.max=65535
    //% shim=LCD1IN8::DrawRectangle
    //% weight=130
    export function DrawRectangle(Xstart2: number, Ystart2: number, Xend2: number, Yend2: number, Color: number, Filled: DRAW_FILL, Dot_Pixel: DOT_PIXEL): void {
    }

    //% blockId=DrawCircle
    //% blockGap=8
    //% block="Draw Circle|X_Center %X_Center|Y_Center %Y_Center|Radius %Radius|Color %Color|Filled %Draw_Fill|Line width %Dot_Pixel"
    //% X_Center.min=1 X_Center.max=160 Y_Center.min=1 Y_Center.max=128
    //% Radius.min=0 Radius.max=160
    //% Color.min=0 Color.max=65535
    //% shim=LCD1IN8::DrawCircle
    //% weight=120
    export function DrawCircle(X_Center: number, Y_Center: number, Radius: number, Color: number, Draw_Fill: DRAW_FILL, Dot_Pixel: DOT_PIXEL): void {
    }

    //% blockId=DisString
    //% blockGap=8
    //% block="Show String|X %Xchar|Y %Ychar|char %ch|Color %Color"
    //% Xchar.min=1 Xchar.max=160 Ychar.min=1 Ychar.max=128 
    //% Color.min=0 Color.max=65535
    //% weight=100
    export function DisString(Xchar: number, Ychar: number, ch: string, Color: number): void{
		let Xpoint = Xchar;
		let Ypoint = Ychar;
        let Font_Height = 12;
        let Font_Width = 7;
		let ch_len = ch.length;
		let i = 0;
		for(i = 0; i < ch_len; i++){
			let ch_asicc =  ch.charCodeAt(i) - 32;//NULL = 32
			let Char_Offset = ch_asicc * 12;
			
			if((Xpoint + Font_Width) > 160) {
				Xpoint = Xchar;
				Ypoint += Font_Height;
			}

			// If the Y direction is full, reposition to(Xstart, Ystart)
			if((Ypoint  + Font_Height) > 128) {
				Xpoint = Xchar;
				Ypoint = Ychar;
			}
			DisChar_1207(Xpoint, Ypoint, Char_Offset, Color);
			
			//The next word of the abscissa increases the font of the broadband
			Xpoint += Font_Width;
		} 
    }
    
    //% blockId=DisNumber
    //% blockGap=8
    //% block="Show number|X %Xnum|Y %Ynum|number %num|Color %Color"
    //% Xnum.min=1 Xnum.max=160 Ynum.min=1 Ynum.max=128 
    //% Color.min=0 Color.max=65535
    //% weight=100
    export function DisNumber(Xnum: number, Ynum: number, num: number, Color: number): void{
        DisString(Xnum, Ynum, num + "", Color);
    }

    //% shim=LCD1IN8::DisChar_1207
    function DisChar_1207(Xchar:number, Ychar:number, Char_Offset:number, Color:number): void {
    }
}
