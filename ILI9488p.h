#ifndef _ILI9488P_H_
#define _ILI9488P_H_

#include "Arduino.h"
#include "Print.h"
#include <Adafruit_GFX.h>

/*
To use a different resolution TFT, change this or use setResolution().
*/
#define TFTWIDTH 240
#define TFTHEIGHT 320

// Color definitions
#define BLACK 0x0000       /*   0,   0,   0 */
#define NAVY 0x000F        /*   0,   0, 128 */
#define DARKGREEN 0x03E0   /*   0, 128,   0 */
#define DARKCYAN 0x03EF    /*   0, 128, 128 */
#define MAROON 0x7800      /* 128,   0,   0 */
#define PURPLE 0x780F      /* 128,   0, 128 */
#define OLIVE 0x7BE0       /* 128, 128,   0 */
#define LIGHTGRAY 0xC618   /* 192, 192, 192 */
#define DARKGRAY 0x7BEF    /* 128, 128, 128 */
#define BLUE 0x001F        /*   0,   0, 255 */
#define GREEN 0x07E0       /*   0, 255,   0 */
#define CYAN 0x07FF        /*   0, 255, 255 */
#define RED 0xF800         /* 255,   0,   0 */
#define MAGENTA 0xF81F     /* 255,   0, 255 */
#define YELLOW 0xFFE0      /* 255, 255,   0 */
#define WHITE 0xFFFF       /* 255, 255, 255 */
#define ORANGE 0xFD20      /* 255, 165,   0 */
#define GREENYELLOW 0xAFE5 /* 173, 255,  47 */
#define PINK 0xF81F
#define GRAY 0x5AEB

/*
Define pins and Output Data Registers
*/

#define TFT_D0 41
#define TFT_D1 40
#define TFT_D2 39
#define TFT_D3 38
#define TFT_D4 37
#define TFT_D5 36
#define TFT_D6 35
#define TFT_D7 34

#define TFT_D8 31
#define TFT_D9 30
#define TFT_D10 29
#define TFT_D11 28
#define TFT_D12 27
#define TFT_D13 26
#define TFT_D14 25
#define TFT_D15 24

#define TFT_RD 8
#define TFT_WR 7
#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST 6

#if F_CPU > 2000000000 // even at 720mhz artifacts show up without delay
#define DELAY delayNanoseconds(1)
#else
#define DELAY (void)0 // NOP

#endif

#define RD_ACTIVE digitalWriteFast(TFT_RD, LOW)
#define RD_IDLE digitalWriteFast(TFT_RD, HIGH)
#define WR_ACTIVE digitalWriteFast(TFT_WR, LOW)
#define WR_IDLE digitalWriteFast(TFT_WR, HIGH)
#define CD_COMMAND digitalWriteFast(TFT_DC, LOW)
#define CD_DATA digitalWriteFast(TFT_DC, HIGH)
#define CS_ACTIVE digitalWriteFast(TFT_CS, LOW)
#define CS_IDLE digitalWriteFast(TFT_CS, HIGH)
#define RST_ACTIVE digitalWriteFast(TFT_RST, LOW)
#define RST_IDLE digitalWriteFast(TFT_RST, HIGH)

#define RD_STROBE \
  {               \
    RD_ACTIVE;    \
    RD_IDLE;      \
  } // Not use
#define WR_STROBE \
  {               \
    WR_ACTIVE;    \
    WR_IDLE;      \
  } // Not use

class ILI9488p : public Adafruit_GFX
{

public:
  ILI9488p(void);
  void setResolution(int16_t width, int16_t height);
  void setOffset(int16_t offset);
  void begin(uint16_t ID);
  void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
  void fillScreen(uint16_t color);
  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
  void drawPixel(int16_t x, int16_t y, uint16_t color);
  void pushColors8(uint8_t *block, int16_t n, bool first);
  void pushColors(uint16_t *block, int16_t n, bool first);
  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
  int16_t readGRAM(int16_t x, int16_t y, uint16_t *block, int16_t w, int16_t h);
  uint16_t readPixel(int16_t x, int16_t y)
  {
    uint16_t color;
    readGRAM(x, y, &color, 1, 1);
    return color;
  }
  void setRotation(uint8_t r);
  void vertScroll(int16_t top, int16_t scrollines, int16_t offset);
  void invertDisplay(boolean i);
  uint16_t readID(void);

private:
  void setWriteDataBus(void);
  void setReadDataBus(void);
  void write8(uint8_t);
  void write16(uint16_t);
  void writeCmdByte(uint8_t c);
  void WriteCmd(uint16_t c);
  void writeDataByte(uint8_t d);
  void writeDataWord(uint16_t d);
  void WriteCmdData(uint16_t cmd, uint16_t dat);
  void WriteCmdParamN(uint8_t cmd, int8_t N, uint8_t *block);
  void WriteCmdParam4(uint8_t cmd, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4);
  void init_table(const void *table, int16_t size);
  void init_table16(const void *table, int16_t size);
  void writeCmdWord(uint16_t c);
  uint8_t read8(void);
  uint16_t read16(void);
  uint8_t read8bits(void);
  uint16_t read16bits(void);
  uint16_t readReg16(uint16_t reg);
  uint16_t readReg16Index(uint16_t reg, int8_t index);
  uint32_t readReg32(uint16_t reg);
  uint32_t readReg40(uint16_t reg);
  uint16_t _lcd_ID, _lcd_capable, _lcd_rev, _lcd_madctl, _lcd_drivOut, _MC, _MP, _MW, _SC, _EC, _SP, _EP;
  int16_t _lcd_width = 0, _lcd_height = 0, _lcd_offset = 0;
  uint32_t _pins[16];
};

#endif // endif of the header file
