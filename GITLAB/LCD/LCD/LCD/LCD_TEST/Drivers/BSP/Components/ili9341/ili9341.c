/**
  ******************************************************************************
  * @file    ili9341.c
  * @author  MCD Application Team
  * @version V1.0.2
  * @date    02-December-2014
  * @brief   This file includes the LCD driver for ILI9341 LCD.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "ili9341.h"

#include "../../../Utilities/Fonts/fonts.h"
#include "../../../Utilities/Fonts/font24.c"
#include "../../../Utilities/Fonts/font20.c"
#include "../../../Utilities/Fonts/font16.c"
#include "../../../Utilities/Fonts/font12.c"
#include "../../../Utilities/Fonts/font8.c"

/** @addtogroup BSP
  * @{
  */ 

/** @addtogroup Components
  * @{
  */ 
  
/** @addtogroup ILI9341
  * @brief This file provides a set of functions needed to drive the 
  *        ILI9341 LCD.
  * @{
  */

/** @defgroup ILI9341_Private_TypesDefinitions
  * @{
  */ 
	#define ABS(X)  ((X) > 0 ? (X) : -(X))
/**
  * @}
  */ 

/** @defgroup ILI9341_Private_Defines
  * @{
  */
/**
  * @}
  */ 
  
/** @defgroup ILI9341_Private_Macros
  * @{
  */
/**
  * @}
  */  

/** @defgroup ILI9341_Private_Variables
  * @{
  */ 
/*FONTS*/
static uint32_t ActiveLayer = 0;
static LCD_DrawPropTypeDef DrawProp[MAX_LAYER_NUMBER];

/*Orientation and Dimension Structure*/
typedef enum {
	TM_ILI9341_Landscape,
	TM_ILI9341_Portrait
} TM_ILI9341_Orientation;

/**
 * @brief  LCD options
 * @note   Used private
 */
typedef struct {
	uint16_t width;
	uint16_t height;
	TM_ILI9341_Orientation orientation; // 1 = portrait; 0 = landscape
} TM_ILI931_Options_t;

LCD_DrvTypeDef   ili9341_drv = 
{
  ili9341_Init,
  ili9341_ReadID,
  ili9341_DisplayOn,
  ili9341_DisplayOff,
  0,
  0,
  0,
  0,
  0,
  0,
  ili9341_GetLcdPixelWidth,
  ili9341_GetLcdPixelHeight,
  0,
  0,    
};


/**
  * @}
  */ 
  
/** @defgroup ILI9341_Private_FunctionPrototypes
  * @{
  */

/**
  * @}
  */ 
  
/** @defgroup ILI9341_Private_Functions
  * @{
  */   
void setCursor(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y);
void drawPixel(uint16_t x, uint16_t y, uint32_t color);
void drawCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color);
void drawLine(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint32_t color);
void SetBackColor(uint32_t Color);
void SetTextColor(uint32_t Color);
static void drawChar(uint16_t Xpos, uint16_t Ypos, const uint8_t *c);
void displayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii,sFONT *pFonts);
sFONT *BSP_LCD_GetFont(void);
void displayStringAt(uint16_t X, uint16_t Y, uint8_t *pText, Text_AlignModeTypdef mode, sFONT *pFonts);
void displayStringAtLine(uint16_t Line, uint8_t *ptr, sFONT *pFonts);
void Lcd_Rotate(TM_ILI9341_Orientation_t orientation);

//-----------------------------------------------------------------------------------------------------
TM_ILI931_Options_t ILI9341_Opts;
//-----------------------------------------------------------------------------------------------------

/**
  * @brief  Power on the LCD.
  * @param  None
  * @retval None
  */
void ili9341_Init(void)
{
  /* Initialize ILI9341 low level bus layer ----------------------------------*/
  LCD_IO_Init();
  ili9341_WriteReg(0x01);
	LCD_Delay(100);
	
	ili9341_WriteReg(LCD_POWERA);
	ili9341_WriteData(0x39);
	ili9341_WriteData(0x2C);
	ili9341_WriteData(0x00);
	ili9341_WriteData(0x34);
	ili9341_WriteData(0x02);
	ili9341_WriteReg(LCD_POWERB);
	ili9341_WriteData(0x00);
	ili9341_WriteData(0xC1);
	ili9341_WriteData(0x30);
	ili9341_WriteReg(LCD_DTCA);
	ili9341_WriteData(0x85);
	ili9341_WriteData(0x00);
	ili9341_WriteData(0x78);
	ili9341_WriteReg(LCD_DTCB);
	ili9341_WriteData(0x00);
	ili9341_WriteData(0x00);
	ili9341_WriteReg(LCD_POWER_SEQ);
	ili9341_WriteData(0x64);
	ili9341_WriteData(0x03);
	ili9341_WriteData(0x12);
	ili9341_WriteData(0x81);
	ili9341_WriteReg(LCD_PRC);
	ili9341_WriteData(0x20);
	ili9341_WriteReg(LCD_POWER1);
	ili9341_WriteData(0x23);
	ili9341_WriteReg(LCD_POWER2);
	ili9341_WriteData(0x10);
	ili9341_WriteReg(LCD_VCOM1);
	ili9341_WriteData(0x3E);
	ili9341_WriteData(0x28);
	ili9341_WriteReg(LCD_VCOM2);
	ili9341_WriteData(0x86);
	ili9341_WriteReg(LCD_MAC);
	ili9341_WriteData(0x48);
	ili9341_WriteReg(LCD_PIXEL_FORMAT);
	ili9341_WriteData(0x55);
	ili9341_WriteReg(0xB1);
	ili9341_WriteData(0x00);
	ili9341_WriteData(0x18);
	ili9341_WriteReg(LCD_DFC);
	ili9341_WriteData(0x08);
	ili9341_WriteData(0x82);
	ili9341_WriteData(0x27);
	ili9341_WriteReg(LCD_3GAMMA_EN);
	ili9341_WriteData(0x00);
	ili9341_WriteReg(LCD_COLUMN_ADDR);
	ili9341_WriteData(0x00);
	ili9341_WriteData(0x00);
	ili9341_WriteData(0x00);
	ili9341_WriteData(0xEF);
	ili9341_WriteReg(LCD_PAGE_ADDR);
	ili9341_WriteData(0x00);
	ili9341_WriteData(0x00);
	ili9341_WriteData(0x01);
	ili9341_WriteData(0x3F);
	ili9341_WriteReg(LCD_GAMMA);
	ili9341_WriteData(0x01);
	ili9341_WriteReg(LCD_PGAMMA);
	ili9341_WriteData(0x0F);
	ili9341_WriteData(0x31);
	ili9341_WriteData(0x2B);
	ili9341_WriteData(0x0C);
	ili9341_WriteData(0x0E);
	ili9341_WriteData(0x08);
	ili9341_WriteData(0x4E);
	ili9341_WriteData(0xF1);
	ili9341_WriteData(0x37);
	ili9341_WriteData(0x07);
	ili9341_WriteData(0x10);
	ili9341_WriteData(0x03);
	ili9341_WriteData(0x0E);
	ili9341_WriteData(0x09);
	ili9341_WriteData(0x00);
	ili9341_WriteReg(LCD_NGAMMA);
	ili9341_WriteData(0x00);
	ili9341_WriteData(0x0E);
	ili9341_WriteData(0x14);
	ili9341_WriteData(0x03);
	ili9341_WriteData(0x11);
	ili9341_WriteData(0x07);
	ili9341_WriteData(0x31);
	ili9341_WriteData(0xC1);
	ili9341_WriteData(0x48);
	ili9341_WriteData(0x08);
	ili9341_WriteData(0x0F);
	ili9341_WriteData(0x0C);
	ili9341_WriteData(0x31);
	ili9341_WriteData(0x36);
	ili9341_WriteData(0x0F);
	ili9341_WriteReg(LCD_SLEEP_OUT);

	LCD_Delay(1000);

	ili9341_WriteReg(LCD_DISPLAY_ON);
	ili9341_WriteReg(LCD_GRAM);

}

/**
  * @brief  Disables the Display.
  * @param  None
  * @retval LCD Register Value.
  */
uint16_t ili9341_ReadID(void)
{
  LCD_IO_Init();
  return ((uint16_t)ili9341_ReadData(LCD_READ_ID4, LCD_READ_ID4_SIZE));
}

/**
  * @brief  Enables the Display.
  * @param  None
  * @retval None
  */
void ili9341_DisplayOn(void)
{
  /* Display On */
  ili9341_WriteReg(LCD_DISPLAY_ON);
}

/**
  * @brief  Disables the Display.
  * @param  None
  * @retval None
  */
void ili9341_DisplayOff(void)
{
  /* Display Off */
  ili9341_WriteReg(LCD_DISPLAY_OFF);
}

/**
  * @brief  Writes  to the selected LCD register.
  * @param  LCD_Reg: address of the selected register.
  * @retval None
  */
void ili9341_WriteReg(uint8_t LCD_Reg)
{
  LCD_IO_WriteReg(LCD_Reg);
}

/**
  * @brief  Writes data to the selected LCD register.
  * @param  LCD_Reg: address of the selected register.
  * @retval None
  */
void ili9341_WriteData(uint16_t RegValue)
{
  LCD_IO_WriteData(RegValue);
}

/**
  * @brief  Reads the selected LCD Register.
  * @param  RegValue: Address of the register to read
  * @param  ReadSize: Number of bytes to read
  * @retval LCD Register Value.
  */
uint32_t ili9341_ReadData(uint16_t RegValue, uint8_t ReadSize)
{
  /* Read a max of 4 bytes */
  return (LCD_IO_ReadData(RegValue, ReadSize));
}

/**
  * @brief  Get LCD PIXEL WIDTH.
  * @param  None
  * @retval LCD PIXEL WIDTH.
  */
uint16_t ili9341_GetLcdPixelWidth(void)
{
  /* Return LCD PIXEL WIDTH */
  return ILI9341_LCD_PIXEL_WIDTH;
}

/**
  * @brief  Get LCD PIXEL HEIGHT.
  * @param  None
  * @retval LCD PIXEL HEIGHT.
  */
uint16_t ili9341_GetLcdPixelHeight(void)
{
  /* Return LCD PIXEL HEIGHT */
  return ILI9341_LCD_PIXEL_HEIGHT;
}

/**
  * @}
  */ 
void setCursor(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
	ili9341_WriteReg(LCD_COLUMN_ADDR);
	ili9341_WriteData(x1 >> 8);
	ili9341_WriteData(x1 & 0xFF);
	ili9341_WriteData(x2 >> 8);
	ili9341_WriteData(x2 & 0xFF);
	
	ili9341_WriteReg(LCD_PAGE_ADDR);
	ili9341_WriteData(y1 >> 8);
  ili9341_WriteData(y1 & 0xFF);
	ili9341_WriteData(y2 >> 8);
	ili9341_WriteData(y2 & 0xFF);
}

/**
  * @}
  */ 
void drawPixel(uint16_t x, uint16_t y, uint32_t color)
{
	setCursor(x, y, x, y);

  ili9341_WriteReg(LCD_GRAM);
	ili9341_WriteData(color >> 8);
	ili9341_WriteData(color & 0xFF);
}

  
/**
  * @}
  */ 

void drawCircle(int16_t x0, int16_t y0, int16_t r, uint32_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;
	
	  drawPixel(x0, y0 + r, color);
    drawPixel(x0, y0 - r, color);
    drawPixel(x0 + r, y0, color);
    drawPixel(x0 - r, y0, color);
	  while (x < y) {
        if (f >= 0) 
					{
            y--;
            ddF_y += 2;
            f += ddF_y;
           }
        x++;
        ddF_x += 2;
        f += ddF_x;

        drawPixel(x0 + x, y0 + y, color);
        drawPixel(x0 - x, y0 + y, color);
        drawPixel(x0 + x, y0 - y, color);
        drawPixel(x0 - x, y0 - y, color);

        drawPixel(x0 + y, y0 + x, color);
        drawPixel(x0 - y, y0 + x, color);
        drawPixel(x0 + y, y0 - x, color);
        drawPixel(x0 - y, y0 - x, color);
    }
}
void drawLine(uint16_t X1, uint16_t Y1, uint16_t X2, uint16_t Y2, uint32_t color)
{
  int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, 
  yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0, 
  curpixel = 0;
  
  deltax = ABS(X2 - X1);        /* The difference between the x's */
  deltay = ABS(Y2 - Y1);        /* The difference between the y's */
  x = X1;                       /* Start x off at the first pixel */
  y = Y1;                       /* Start y off at the first pixel */
  
  if (X2 >= X1)                 /* The x-values are increasing */
  {
    xinc1 = 1;
    xinc2 = 1;
  }
  else                          /* The x-values are decreasing */
  {
    xinc1 = -1;
    xinc2 = -1;
  }
  
  if (Y2 >= Y1)                 /* The y-values are increasing */
  {
    yinc1 = 1;
    yinc2 = 1;
  }
  else                          /* The y-values are decreasing */
  {
    yinc1 = -1;
    yinc2 = -1;
  }
  
  if (deltax >= deltay)         /* There is at least one x-value for every y-value */
  {
    xinc1 = 0;                  /* Don't change the x when numerator >= denominator */
    yinc2 = 0;                  /* Don't change the y for every iteration */
    den = deltax;
    num = deltax / 2;
    numadd = deltay;
    numpixels = deltax;         /* There are more x-values than y-values */
  }
  else                          /* There is at least one y-value for every x-value */
  {
    xinc2 = 0;                  /* Don't change the x for every iteration */
    yinc1 = 0;                  /* Don't change the y when numerator >= denominator */
    den = deltay;
    num = deltay / 2;
    numadd = deltax;
    numpixels = deltay;         /* There are more y-values than x-values */
  }
  
  for (curpixel = 0; curpixel <= numpixels; curpixel++)
  {
    drawPixel(x, y, color);   /* Draw the current pixel */
    num += numadd;                            /* Increase the numerator by the top of the fraction */
    if (num >= den)                           /* Check if numerator >= denominator */
    {
      num -= den;                             /* Calculate the new numerator value */
      x += xinc1;                             /* Change the x as appropriate */
      y += yinc1;                             /* Change the y as appropriate */
    }
    x += xinc2;                               /* Change the x as appropriate */
    y += yinc2;                               /* Change the y as appropriate */
  }
}

void SetTextColor(uint32_t Color)
{
  DrawProp[ActiveLayer].TextColor = Color;
}

void SetBackColor(uint32_t Color)
{
  DrawProp[ActiveLayer].BackColor = Color;
}


static void drawChar(uint16_t Xpos, uint16_t Ypos, const uint8_t *c)
{
  uint32_t i = 0, j = 0;
  uint16_t height, width;
  uint8_t offset;
  uint8_t *pchar;
  uint32_t line=0;
  height = DrawProp[ActiveLayer].pFont->Height;
  width  = DrawProp[ActiveLayer].pFont->Width;

  offset = 8 *((width + 7)/8) -  width ;

  for(i = 0; i < height; i++)
  {
    pchar = ((uint8_t *)c + (width + 7)/8 * i);

    switch(((width + 7)/8))
    {
    case 1:
      line =  pchar[0];      
      break;
      
    case 2:
      line =  (pchar[0]<< 8) | pchar[1];
      break;

    case 3:
    default:
      line =  (pchar[0]<< 16) | (pchar[1]<< 8) | pchar[2];      
      break;
    }

    for (j = 0; j < width; j++)
    {
      if(line & (1 << (width- j + offset- 1))) 
      {
				//DrawProp[ActiveLayer].TextColor
        drawPixel((Xpos + j), Ypos, DrawProp[ActiveLayer].TextColor);
      }
      else
      {
				//DrawProp[ActiveLayer].BackColor
        drawPixel((Xpos + j), Ypos, DrawProp[ActiveLayer].BackColor);
      } 
    }
    Ypos++;
  }
}

void displayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii,sFONT *pFonts)
{
	DrawProp[ActiveLayer].pFont = pFonts;
  drawChar(Xpos, Ypos, &DrawProp[ActiveLayer].pFont->table[(Ascii-' ') *\
              DrawProp[ActiveLayer].pFont->Height * ((DrawProp[ActiveLayer].pFont->Width + 7) / 8)]);
}

sFONT *BSP_LCD_GetFont(void)
{
  return DrawProp[ActiveLayer].pFont;
}
void displayStringAt(uint16_t X, uint16_t Y, uint8_t *pText, Text_AlignModeTypdef mode, sFONT *pFonts)
{
	uint16_t xaxis=X,yaxis=Y;
  uint16_t refcolumn = 1, i = 0;
  uint32_t size = 0, xsize = 0; 
  uint8_t  *ptr = pText;
  
  /* Get the text size */ 
  while (*ptr++) size ++ ;
  
  /* Characters number per line */
  //xsize = (BSP_LCD_GetXSize()/DrawProp[ActiveLayer].pFont->Width);
  xsize = (240/DrawProp[ActiveLayer].pFont->Width);
  

  switch (mode)
  {
  case CENTER_MODE:
    {
      refcolumn = X+ ((xsize - size)* DrawProp[ActiveLayer].pFont->Width) / 2;
      break;
    }
  case LEFT_MODE:
    {
      refcolumn = X;
      break;
    }
  case RIGHT_MODE:
    {
      refcolumn = X + ((xsize - size)*DrawProp[ActiveLayer].pFont->Width);
      break;
    }
  default:
    {
      refcolumn = X;
      break;
    }
  }

  /* Send the string character by character on LCD */
  //  while ((*pText != 0) & (((BSP_LCD_GetXSize() - (i*DrawProp[ActiveLayer].pFont->Width)) & 0xFFFF) >= DrawProp[ActiveLayer].pFont->Width))
  	displayChar(0,0,' ',&Font8);
//	pText++;
  while ((*pText != 0) & (((240 - (i*DrawProp[ActiveLayer].pFont->Width)) & 0xFFFF) >= DrawProp[ActiveLayer].pFont->Width))
  {
    /* Display one character on LCD */
    displayChar(refcolumn, Y, *pText,pFonts);
    /* Decrement the column position by 16 */
    refcolumn += DrawProp[ActiveLayer].pFont->Width;
    /* Point on the next character */
    pText++;
    i++;
  }  
}
void displayStringAtLine(uint16_t Line, uint8_t *ptr,sFONT *pFonts)
{
  displayStringAt(0, LINE(Line), ptr, LEFT_MODE, pFonts);
}

void Lcd_Rotate(TM_ILI9341_Orientation_t orientation) {
  ili9341_WriteReg(LCD_MAC);
  if (orientation == TM_ILI9341_Orientation_Portrait_1) {
    ili9341_WriteData(0x58);
  } else if (orientation == TM_ILI9341_Orientation_Portrait_2) {
    ili9341_WriteData(0x88);
  } else if (orientation == TM_ILI9341_Orientation_Landscape_1) {
    ili9341_WriteData(0x28);
  } else if (orientation == TM_ILI9341_Orientation_Landscape_2) {
    ili9341_WriteData(0xE8);
  }
  
  if (orientation == TM_ILI9341_Orientation_Portrait_1 || orientation == TM_ILI9341_Orientation_Portrait_2) {
    ILI9341_Opts.width = ILI9341_LCD_PIXEL_WIDTH;
    ILI9341_Opts.height = ILI9341_LCD_PIXEL_HEIGHT;
    ILI9341_Opts.orientation = TM_ILI9341_Portrait;
  } else {
    ILI9341_Opts.width = ILI9341_LCD_PIXEL_HEIGHT;
    ILI9341_Opts.height = ILI9341_LCD_PIXEL_WIDTH;
    ILI9341_Opts.orientation = TM_ILI9341_Landscape;
  }
}

/**
  * @}
  */
  
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
