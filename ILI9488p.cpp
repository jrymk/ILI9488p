/*
 * 8bit TFT Library for Arduino_Core_STM32
 *
 * based on MCUFRIEND_kbv.cpp by David Prentice
 * https://github.com/prenticedavid/MCUFRIEND_kbv

 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <ILI9488p.h>

#define MIPI_DCS_REV1 (1 << 0)
#define AUTO_READINC (1 << 1)
#define READ_BGR (1 << 2)
#define READ_LOWHIGH (1 << 3)
#define READ_24BITS (1 << 4)
#define XSA_XEA_16BIT (1 << 5)
#define READ_NODUMMY (1 << 6)
#define INVERT_GS (1 << 8)
#define INVERT_SS (1 << 9)
#define MV_AXIS (1 << 10)
#define INVERT_RGB (1 << 11)
#define REV_SCREEN (1 << 12)
#define FLIP_VERT (1 << 13)
#define FLIP_HORIZ (1 << 14)

static uint8_t is8347 = 0;

// Constructor when using 8080 mode of control.
ILI9488p::ILI9488p(void)
    : Adafruit_GFX(TFTWIDTH, TFTHEIGHT)
{

  // set control pin mode
  pinMode(TFT_RD, OUTPUT);
  pinMode(TFT_WR, OUTPUT);
  pinMode(TFT_DC, OUTPUT);
  pinMode(TFT_CS, OUTPUT);
  pinMode(TFT_RST, OUTPUT);

  

  CS_IDLE;  // Disable CS
  CD_DATA;  // Enable Command
  WR_IDLE;  // Disable WR
  RD_IDLE;  // Disable RD
  RST_IDLE; // Disable RST

  // toggle RST low to reset
  RST_ACTIVE; // Set Reset LOW
  delay(20);
  RST_IDLE; // Set Reset HIGH
  delay(150);

  _pins[0] = TFT_D0;
  _pins[1] = TFT_D1;
  _pins[2] = TFT_D2;
  _pins[3] = TFT_D3;
  _pins[4] = TFT_D4;
  _pins[5] = TFT_D5;
  _pins[6] = TFT_D6;
  _pins[7] = TFT_D7;
}

void ILI9488p::setWriteDataBus(void)
{
  for (int i = 0; i < 8; i++)
  {
    pinMode(_pins[i], OUTPUT);
  }
}

void ILI9488p::setReadDataBus(void)
{
  for (int i = 0; i < 8; i++)
  {
    pinMode(_pins[i], INPUT);
  }
}

void ILI9488p::write8(uint8_t bytes)
{
  for (int i = 0; i < 8; i++)
  {
    digitalWriteFast(_pins[i], (bytes >> i) & 1);
  }
}

void ILI9488p::writeCmdByte(uint8_t c)
{
  CS_ACTIVE;
  CD_COMMAND;
  RD_IDLE;
  WR_IDLE;

  write8(c);
  WR_ACTIVE;
  DELAY;
  WR_IDLE;
  CS_IDLE;
}

void ILI9488p::writeCmdWord(uint16_t c)
{
  CS_ACTIVE;
  CD_COMMAND;
  RD_IDLE;
  WR_IDLE;

  write8(c >> 8);
  WR_ACTIVE;
  DELAY;
  WR_IDLE;

  write8(c & 0xff);
  WR_ACTIVE;
  DELAY;
  WR_IDLE;
  CS_IDLE;
}

void ILI9488p::WriteCmd(uint16_t c)
{
  //  if (_lcd_cmdWidth == 8) writeCmdByte(c & 0xff);
  //  if (_lcd_cmdWidth == 16) writeCmdWord(c);
  if (_lcd_capable & MIPI_DCS_REV1)
  {
    writeCmdByte(c & 0xff);
  }
  else
  {
    writeCmdWord(c);
  }
}

void ILI9488p::writeDataByte(uint8_t c)
{
  CS_ACTIVE;
  CD_DATA;
  RD_IDLE;
  WR_IDLE;

  write8(c);
  WR_ACTIVE;
  DELAY;
  WR_IDLE;
  CS_IDLE;
}

void ILI9488p::writeDataWord(uint16_t c)
{
  CS_ACTIVE;
  CD_DATA;
  RD_IDLE;
  WR_IDLE;

  write8(c >> 8);
  WR_ACTIVE;
  DELAY;
  WR_IDLE;

  write8(c & 0xff);
  WR_ACTIVE;
  DELAY;
  WR_IDLE;
  CS_IDLE;
}

void ILI9488p::WriteCmdData(uint16_t cmd, uint16_t dat)
{
  WriteCmd(cmd);
  writeDataWord(dat);
}

void ILI9488p::WriteCmdParamN(uint8_t cmd, int8_t N, uint8_t *block)
{
  writeCmdByte(cmd);
  while (N-- > 0)
  {
    uint8_t u8 = *block++;
    writeDataByte(u8);
  }
}

void ILI9488p::WriteCmdParam4(uint8_t cmd, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4)
{
  uint8_t d[4];
  d[0] = d1, d[1] = d2, d[2] = d3, d[3] = d4;
  WriteCmdParamN(cmd, 4, d);
}

#define TFTLCD_DELAY 0xFFFF
#define TFTLCD_DELAY8 0x7F

void ILI9488p::init_table(const void *table, int16_t size)
{
  uint8_t *p = (uint8_t *)table, dat[24]; // R61526 has GAMMA[22]
  while (size > 0)
  {
    uint8_t cmd = pgm_read_byte(p++);
    uint8_t len = pgm_read_byte(p++);
    if (cmd == TFTLCD_DELAY8)
    {
      delay(len);
      len = 0;
    }
    else
    {
      for (uint8_t i = 0; i < len; i++)
        dat[i] = pgm_read_byte(p++);
      WriteCmdParamN(cmd, len, dat);
    }
    size -= len + 2;
  }
}

void ILI9488p::init_table16(const void *table, int16_t size)
{
  uint16_t *p = (uint16_t *)table;
  while (size > 0)
  {
    uint16_t cmd = pgm_read_word(p++);
    uint16_t d = pgm_read_word(p++);
    if (cmd == TFTLCD_DELAY)
      delay(d);
    else
    {
      writeCmdWord(cmd);
      writeDataWord(d);
    }
    size -= 2 * sizeof(int16_t);
  }
}

// The default resolution is 240x320.
// Use this function for other resolutions.
void ILI9488p::setResolution(int16_t width, int16_t height)
{
  _lcd_width = width;
  _lcd_height = height;
}

void ILI9488p::setOffset(int16_t offset)
{
  _lcd_offset = offset;
}

void ILI9488p::begin(uint16_t ID)
{

  int16_t *p16; // so we can "write" to a const protected variable.
  const uint8_t *table8_ads = NULL;
  int16_t table_size;

  // set up 8 bit parallel port to write mode.
  setWriteDataBus();

  // Serial.println("_lcd_width=" + String(_lcd_width) + " _lcd_height=" + String(_lcd_height));
  // Serial.println("_lcd_offset=" + String(_lcd_offset));

  _lcd_ID = ID;
  // Serial.print("_lcd_ID=");
  // Serial.println(_lcd_ID, HEX);

  // this was for 9481, but 9488 basically the same I guess?
  _lcd_capable = AUTO_READINC | MIPI_DCS_REV1 | MV_AXIS | READ_BGR;
common_9481:
  static const uint8_t ILI9481_regValues[] PROGMEM = {
      // Atmel MaxTouch
      0xB0, 1, 0x00,                         // unlocks E0, F0
      0xB3, 4, 0x02, 0x00, 0x00, 0x00,       // Frame Memory, interface [02 00 00 00]
      0xB4, 1, 0x00,                         // Frame mode [00]
      0xD0, 3, 0x07, 0x42, 0x18,             // Set Power [00 43 18] x1.00, x6, x3
      0xD1, 3, 0x00, 0x07, 0x10,             // Set VCOM  [00 00 00] x0.72, x1.02
      0xD2, 2, 0x01, 0x02,                   // Set Power for Normal Mode [01 22]
      0xD3, 2, 0x01, 0x02,                   // Set Power for Partial Mode [01 22]
      0xD4, 2, 0x01, 0x02,                   // Set Power for Idle Mode [01 22]
      0xC0, 5, 0x12, 0x3B, 0x00, 0x02, 0x11, // Panel Driving BGR for 1581 [10 3B 00 02 11]
      0xC1, 3, 0x10, 0x10, 0x88,             // Display Timing Normal [10 10 88]
      0xC5, 1, 0x03,                         // Frame Rate [03]
      0xC6, 1, 0x02,                         // Interface Control [02]
      0xC8, 12, 0x00, 0x32, 0x36, 0x45, 0x06, 0x16, 0x37, 0x75, 0x77, 0x54, 0x0C, 0x00,
      0xCC, 1, 0x00, // Panel Control [00]
  };
  static const uint8_t ILI9481_CPT29_regValues[] PROGMEM = {
      // 320x430
      0xB0,
      1,
      0x00,
      0xD0,
      3,
      0x07,
      0x42,
      0x1C, // Set Power [00 43 18]
      0xD1,
      3,
      0x00,
      0x02,
      0x0F, // Set VCOM  [00 00 00] x0.695, x1.00
      0xD2,
      2,
      0x01,
      0x11, // Set Power for Normal Mode [01 22]
      0xC0,
      5,
      0x10,
      0x35,
      0x00,
      0x02,
      0x11, // Set Panel Driving [10 3B 00 02 11]
      0xC5,
      1,
      0x03, // Frame Rate [03]
      0xC8,
      12,
      0x00,
      0x30,
      0x36,
      0x45,
      0x04,
      0x16,
      0x37,
      0x75,
      0x77,
      0x54,
      0x0F,
      0x00,
      0xE4,
      1,
      0xA0,
      0xF0,
      1,
      0x01,
      0xF3,
      2,
      0x02,
      0x1A,
  };
  static const uint8_t ILI9481_PVI35_regValues[] PROGMEM = {
      // 320x480
      0xB0,
      1,
      0x00,
      0xD0,
      3,
      0x07,
      0x41,
      0x1D, // Set Power [00 43 18]
      0xD1,
      3,
      0x00,
      0x2B,
      0x1F, // Set VCOM  [00 00 00] x0.900, x1.32
      0xD2,
      2,
      0x01,
      0x11, // Set Power for Normal Mode [01 22]
      0xC0,
      5,
      0x10,
      0x3B,
      0x00,
      0x02,
      0x11, // Set Panel Driving [10 3B 00 02 11]
      0xC5,
      1,
      0x03, // Frame Rate [03]
      0xC8,
      12,
      0x00,
      0x14,
      0x33,
      0x10,
      0x00,
      0x16,
      0x44,
      0x36,
      0x77,
      0x00,
      0x0F,
      0x00,
      0xE4,
      1,
      0xA0,
      0xF0,
      1,
      0x01,
      0xF3,
      2,
      0x40,
      0x0A,
  };
  static const uint8_t ILI9481_AUO317_regValues[] PROGMEM = {
      // 320x480
      0xB0,
      1,
      0x00,
      0xD0,
      3,
      0x07,
      0x40,
      0x1D, // Set Power [00 43 18]
      0xD1,
      3,
      0x00,
      0x18,
      0x13, // Set VCOM  [00 00 00] x0.805, x1.08
      0xD2,
      2,
      0x01,
      0x11, // Set Power for Normal Mode [01 22]
      0xC0,
      5,
      0x10,
      0x3B,
      0x00,
      0x02,
      0x11, // Set Panel Driving [10 3B 00 02 11]
      0xC5,
      1,
      0x03, // Frame Rate [03]
      0xC8,
      12,
      0x00,
      0x44,
      0x06,
      0x44,
      0x0A,
      0x08,
      0x17,
      0x33,
      0x77,
      0x44,
      0x08,
      0x0C,
      0xE4,
      1,
      0xA0,
      0xF0,
      1,
      0x01,
  };
  static const uint8_t ILI9481_CMO35_regValues[] PROGMEM = {
      // 320480
      0xB0,
      1,
      0x00,
      0xD0,
      3,
      0x07,
      0x41,
      0x1D, // Set Power [00 43 18] 07,41,1D
      0xD1,
      3,
      0x00,
      0x1C,
      0x1F, // Set VCOM  [00 00 00] x0.825, x1.32 1C,1F
      0xD2,
      2,
      0x01,
      0x11, // Set Power for Normal Mode [01 22]
      0xC0,
      5,
      0x10,
      0x3B,
      0x00,
      0x02,
      0x11, // Set Panel Driving [10 3B 00 02 11]
      0xC5,
      1,
      0x03, // Frame Rate [03]
      0xC6,
      1,
      0x83,
      0xC8,
      12,
      0x00,
      0x26,
      0x21,
      0x00,
      0x00,
      0x1F,
      0x65,
      0x23,
      0x77,
      0x00,
      0x0F,
      0x00,
      0xF0,
      1,
      0x01, //?
      0xE4,
      1,
      0xA0, //?SETCABC on Himax
      0x36,
      1,
      0x48, // Memory Access [00]
      0xB4,
      1,
      0x11,
  };
  static const uint8_t ILI9481_RGB_regValues[] PROGMEM = {
      // 320x480
      0xB0, 1, 0x00,
      0xD0, 3, 0x07, 0x41, 0x1D,                   // SETPOWER [00 43 18]
      0xD1, 3, 0x00, 0x2B, 0x1F,                   // SETVCOM  [00 00 00] x0.900, x1.32
      0xD2, 2, 0x01, 0x11,                         // SETNORPOW for Normal Mode [01 22]
      0xC0, 6, 0x10, 0x3B, 0x00, 0x02, 0x11, 0x00, // SETPANEL [10 3B 00 02 11]
      0xC5, 1, 0x03,                               // SETOSC Frame Rate [03]
      0xC6, 1, 0x80,                               // SETRGB interface control
      0xC8, 12, 0x00, 0x14, 0x33, 0x10, 0x00, 0x16, 0x44, 0x36, 0x77, 0x00, 0x0F, 0x00,
      0xF3, 2, 0x40, 0x0A,
      0xF0, 1, 0x08,
      0xF6, 1, 0x84,
      0xF7, 1, 0x80,
      0x0C, 2, 0x00, 0x55, // RDCOLMOD
      0xB4, 1, 0x00,       // SETDISPLAY
      //            0xB3, 4, 0x00, 0x01, 0x06, 0x01,  //SETGRAM simple example
      0xB3, 4, 0x00, 0x01, 0x06, 0x30, // jpegs example
  };
  table8_ads = ILI9481_regValues, table_size = sizeof(ILI9481_regValues);
  //        table8_ads = ILI9481_CPT29_regValues, table_size = sizeof(ILI9481_CPT29_regValues);
  //        table8_ads = ILI9481_PVI35_regValues, table_size = sizeof(ILI9481_PVI35_regValues);
  //        table8_ads = ILI9481_AUO317_regValues, table_size = sizeof(ILI9481_AUO317_regValues);
  //        table8_ads = ILI9481_CMO35_regValues, table_size = sizeof(ILI9481_CMO35_regValues);
  //        table8_ads = ILI9481_RGB_regValues, table_size = sizeof(ILI9481_RGB_regValues);

  // Set screen resolution
  if (_lcd_width != 0 || _lcd_height != 0)
  {
    p16 = (int16_t *)&HEIGHT;
    *p16 = _lcd_height;
    p16 = (int16_t *)&WIDTH;
    *p16 = _lcd_width;
  }

  if (table8_ads != NULL)
  {
    static const uint8_t reset_off[] PROGMEM = {
        0x01, 0,            // Soft Reset
        TFTLCD_DELAY8, 150, // .kbv will power up with ONLY reset, sleep out, display on
        0x28, 0,            // Display Off
        0x3A, 1, 0x55,      // Pixel read=565, write=565.
    };
    static const uint8_t wake_on[] PROGMEM = {
        0x11, 0, // Sleep Out
        TFTLCD_DELAY8, 150,
        0x29, 0, // Display On
    };

    init_table(&reset_off, sizeof(reset_off));
    init_table(table8_ads, table_size); // can change PIXFMT
    init_table(&wake_on, sizeof(wake_on));
  }

  setRotation(0); // PORTRAIT
  invertDisplay(false);
}

void ILI9488p::drawPixel(int16_t x, int16_t y, uint16_t color)
{
  // MCUFRIEND just plots at edge if you try to write outside of the box:
  if (x < 0 || y < 0 || x >= width() || y >= height())
    return;
  if (_lcd_offset)
  {
    if (rotation == 2)
      y += _lcd_offset;
    if (rotation == 3)
      x += _lcd_offset;
  }

#ifdef _DEBUG_
  Serial.print("drawPixel:x=" + String(x) + " y=" + String(y) + " color=" + String(color, HEX));
  Serial.print(" _MC=" + String(_MC, HEX));
  Serial.print(" _MP=" + String(_MP, HEX));
  Serial.println(" _MW=" + String(_MW, HEX));
#endif

  if (_lcd_capable & MIPI_DCS_REV1)
  {
    WriteCmdParam4(_MC, x >> 8, x, x >> 8, x);
    WriteCmdParam4(_MP, y >> 8, y, y >> 8, y);
  }
  else
  {
    WriteCmdData(_MC, x);
    WriteCmdData(_MP, y);
  }
  WriteCmdData(_MW, color);
}

void ILI9488p::setAddrWindow(uint16_t x, uint16_t y, uint16_t x1, uint16_t y1)
{

  //  Serial.println("x=" + String(x) + " y=" + String(y));
  //  Serial.println("x1=" + String(x1) + " y1=" + String(y1));
  if (_lcd_offset)
  {
    if (rotation == 2)
    {
      y += _lcd_offset;
      y1 += _lcd_offset;
    }
    if (rotation == 3)
    {
      x += _lcd_offset;
      x1 += _lcd_offset;
    }
  }

  if (_lcd_capable & MIPI_DCS_REV1)
  {
    WriteCmdParam4(_MC, x >> 8, x, x1 >> 8, x1);
    WriteCmdParam4(_MP, y >> 8, y, y1 >> 8, y1);
  }
  else
  {
    WriteCmdData(_MC, x);
    WriteCmdData(_MP, y);
    if (_lcd_capable & XSA_XEA_16BIT)
    {
      if (rotation & 1)
        y1 = y = (y1 << 8) | y;
      else
        x1 = x = (x1 << 8) | x;
    }
    WriteCmdData(_SC, x);
    WriteCmdData(_SP, y);
    WriteCmdData(_EC, x1);
    WriteCmdData(_EP, y1);
  }
  //  WriteCmd(_MW); // write to RAM
}

void ILI9488p::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
  int16_t end;
  if (w < 0)
  {
    w = -w;
    x -= w;
  } //+ve w
  end = x + w;
  if (x < 0)
    x = 0;
  if (end > width())
    end = width();
  w = end - x;
  if (h < 0)
  {
    h = -h;
    y -= h;
  } //+ve h
  end = y + h;
  if (y < 0)
    y = 0;
  if (end > height())
    end = height();
  h = end - y;

  setAddrWindow(x, y, x + w - 1, y + h - 1);
  WriteCmd(_MW);
  if (h > w)
  {
    end = h;
    h = w;
    w = end;
  }
  while (h-- > 0)
  {
    end = w;
    do
    {
      writeDataWord(color);
    } while (--end != 0);
  }
  if (!(_lcd_capable & MIPI_DCS_REV1) || ((_lcd_ID == 0x1526) && (rotation & 1)))
    setAddrWindow(0, 0, width() - 1, height() - 1);
}

void ILI9488p::pushColors8(uint8_t *block, int16_t n, bool first)
{
  uint16_t color;
  uint8_t h, l;

  if (first)
  {
    WriteCmd(_MW);
  }
  while (n-- > 0)
  {
    h = (*block++);
    l = (*block++);
    color = h << 8 | l;
    writeDataWord(color);
  }
}

void ILI9488p::pushColors(uint16_t *block, int16_t n, bool first)
{
  uint16_t color;
  uint8_t h, l;

  if (first)
  {
    WriteCmd(_MW);
  }
  while (n-- > 0)
  {
    color = (*block++);
    writeDataWord(color);
  }
}

void ILI9488p::drawFastVLine(int16_t x, int16_t y, int16_t h,
                             uint16_t color)
{

  // Rudimentary clipping
  if ((x >= _width) || (y >= _height || h < 1))
    return;
  if ((y + h - 1) >= _height)
    h = _height - y;
  fillRect(x, y, 1, h, color);
}

void ILI9488p::drawFastHLine(int16_t x, int16_t y, int16_t w,
                             uint16_t color)
{

  // Rudimentary clipping
  if ((x >= _width) || (y >= _height || w < 1))
    return;
  if ((x + w - 1) >= _width)
    w = _width - x;
  fillRect(x, y, w, 1, color);
}

void ILI9488p::fillScreen(uint16_t color)
{
  fillRect(0, 0, _width, _height, color);
}

/*
 * Draw lines faster by calculating straight sections and drawing them with fastVline and fastHline.
 */
void ILI9488p::drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color)
{

  int16_t i;
  int16_t dx, dy;
  int16_t sx, sy;
  int16_t E;
#ifdef _DEBUG_
  Serial.println("drawLine:x1=" + String(x1) + " x2=" + String(x2) + " y1=" + String(y1) + " y2=" + String(y2));
#endif
  /* distance between two points */
  dx = (x2 > x1) ? x2 - x1 : x1 - x2;
  dy = (y2 > y1) ? y2 - y1 : y1 - y2;

  /* direction of two point */
  sx = (x2 > x1) ? 1 : -1;
  sy = (y2 > y1) ? 1 : -1;

  if (y1 == y2)
  {
    if (x2 > x1)
    {
      drawFastHLine(x1, y1, x2 - x1 + 1, color);
    }
    else
    {
      drawFastHLine(x2, y1, x1 - x2 + 1, color);
    }
    return;
  }
  else if (x1 == x2)
  {
    if (y2 > y1)
    {
      drawFastVLine(x1, y1, y2 - y1 + 1, color);
    }
    else
    {
      drawFastVLine(x1, y2, y1 - y2 + 1, color);
    }
    return;
  }

  /* inclination < 1 */
  if (dx > dy)
  {
    E = -dx;
    for (i = 0; i <= dx; i++)
    {
      drawPixel(x1, y1, color);
      x1 += sx;
      E += 2 * dy;
      if (E >= 0)
      {
        y1 += sy;
        E -= 2 * dx;
      }
    }
    /* inclination >= 1 */
  }
  else
  {
    E = -dy;
    for (i = 0; i <= dy; i++)
    {
      drawPixel(x1, y1, color);
      y1 += sy;
      E += 2 * dx;
      if (E >= 0)
      {
        x1 += sx;
        E -= 2 * dy;
      }
    }
  }
}

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t ILI9488p::color565(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// independent cursor and window registers.   ILI9341/9342 auto increments.  ILI9320/9325 do not.
// VERY VERY SLOW !!
int16_t ILI9488p::readGRAM(int16_t x, int16_t y, uint16_t *block, int16_t w, int16_t h)
{
  uint16_t ret, dummy, _MR = _MW;
  int16_t n = w * h, row = 0, col = 0;
  uint8_t r, g, b, tmp;
  if (!is8347 && _lcd_capable & MIPI_DCS_REV1) // HX8347 uses same register
    _MR = 0x2E;
  setAddrWindow(x, y, x + w - 1, y + h - 1);
  while (n > 0)
  {
    if (!(_lcd_capable & MIPI_DCS_REV1))
    {
      WriteCmdData(_MC, x + col);
      WriteCmdData(_MP, y + row);
    }
    WriteCmd(_MR);
    setReadDataBus();
    CS_ACTIVE;
    CD_DATA;
    WR_IDLE;
    RD_ACTIVE;

    if (_lcd_capable & READ_NODUMMY)
    {
      ;
    }
    else if ((_lcd_capable & MIPI_DCS_REV1) || _lcd_ID == 0x1289)
    {
      r = read8();
    }
    else
    {
      dummy = read16bits();
    }
    if (_lcd_ID == 0x1511)
      r = read8(); // extra dummy for R61511
    while (n)
    {
      if (_lcd_capable & READ_24BITS)
      {
        r = read8();
        g = read8();
        b = read8();
        if (_lcd_capable & READ_BGR)
          ret = color565(b, g, r);
        else
          ret = color565(r, g, b);
      }
      else
      {
        ret = read16bits();
        if (_lcd_capable & READ_LOWHIGH)
          ret = (ret >> 8) | (ret << 8);
        if (_lcd_capable & READ_BGR)
          ret = (ret & 0x07E0) | (ret >> 11) | (ret << 11);
      }
      *block++ = ret;
      n--;
      if (!(_lcd_capable & AUTO_READINC))
        break;
    }
    if (++col >= w)
    {
      col = 0;
      if (++row >= h)
        row = 0;
    }
    RD_IDLE;
    CS_IDLE;
    setWriteDataBus();
  }
  if (!(_lcd_capable & MIPI_DCS_REV1))
    setAddrWindow(0, 0, width() - 1, height() - 1);
  return 0;
}

void ILI9488p::setRotation(uint8_t r)
{
  uint16_t GS, SS, ORG, REV = _lcd_rev;
  uint8_t val, d[3];
  rotation = r & 3; // just perform the operation ourselves on the protected variables
  _width = (rotation & 1) ? HEIGHT : WIDTH;
  _height = (rotation & 1) ? WIDTH : HEIGHT;

#ifdef _DEBUG_
  Serial.println("setRotation:r=" + String(r));
  Serial.println("setRotation:rotation=" + String(rotation));
  Serial.println("setRotation:WIDTH=" + String(WIDTH));
  Serial.println("setRotation:HEIGHT=" + String(HEIGHT));
  Serial.println("setRotation:_width=" + String(_width));
  Serial.println("setRotation:_height=" + String(_height));
  Serial.println("setRotation:width()=" + String(width()));
  Serial.println("setRotation:height()=" + String(height()));
#endif
  switch (rotation)
  {
  case 0:       // PORTRAIT:
    val = 0x48; // MY=0, MX=1, MV=0, ML=0, BGR=1
    break;
  case 1:       // LANDSCAPE: 90 degrees
    val = 0x28; // MY=0, MX=0, MV=1, ML=0, BGR=1
    break;
  case 2:       // PORTRAIT_REV: 180 degrees
    val = 0x98; // MY=1, MX=0, MV=0, ML=1, BGR=1
    break;
  case 3:       // LANDSCAPE_REV: 270 degrees
    val = 0xF8; // MY=1, MX=1, MV=1, ML=1, BGR=1
    break;
  }

  if (_lcd_capable & INVERT_GS)
    val ^= 0x80;
  if (_lcd_capable & INVERT_SS)
    val ^= 0x40;
  if (_lcd_capable & INVERT_RGB)
    val ^= 0x08;

  if (_lcd_capable & MIPI_DCS_REV1)
  {
    if (_lcd_ID == 0x6814)
    {                                   //.kbv my weird 0x9486 might be 68140
      GS = (val & 0x80) ? (1 << 6) : 0; // MY
      SS = (val & 0x40) ? (1 << 5) : 0; // MX
      val &= 0x28;                      // keep MV, BGR, MY=0, MX=0, ML=0
      d[0] = 0;
      d[1] = GS | SS | 0x02; // MY, MX
      d[2] = 0x3B;
      WriteCmdParamN(0xB6, 3, d);
      goto common_MC;
    }
    else if (_lcd_ID == 0x1963 || _lcd_ID == 0x9481 || _lcd_ID == 0x1511)
    {
      if (val & 0x80)
        val |= 0x01; // GS
      if ((val & 0x40))
        val |= 0x02; // SS
      if (_lcd_ID == 0x1963)
        val &= ~0xC0;
      if (_lcd_ID == 0x9481)
        val &= ~0xD0;
      if (_lcd_ID == 0x1511)
      {
        val &= ~0x10; // remove ML
        val |= 0xC0;  // force penguin 180 rotation
      }
      //            val &= (_lcd_ID == 0x1963) ? ~0xC0 : ~0xD0; //MY=0, MX=0 with ML=0 for ILI9481
      goto common_MC;
    }
    else if (is8347)
    {
      _MC = 0x02, _MP = 0x06, _MW = 0x22, _SC = 0x02, _EC = 0x04, _SP = 0x06, _EP = 0x08;
      if (_lcd_ID == 0x5252)
      {
        val |= 0x02; // VERT_SCROLLON
        if (val & 0x10)
          val |= 0x04; // if (ML) SS=1 kludge mirror in XXX_REV modes
      }
      goto common_BGR;
    }
  common_MC:
    _MC = 0x2A, _MP = 0x2B, _MW = 0x2C, _SC = 0x2A, _EC = 0x2A, _SP = 0x2B, _EP = 0x2B;
  common_BGR:
    WriteCmdParamN(is8347 ? 0x16 : 0x36, 1, &val);
    _lcd_madctl = val;
    //       if (_lcd_ID    == 0x1963) WriteCmdParamN(0x13, 0, NULL);   //NORMAL mode
  }

  else
  {
    uint16_t GS, SS, ORG, REV = _lcd_rev;
    switch (_lcd_ID)
    {
    case 0x5420:
    case 0x7793:
    case 0x9326:
    case 0xB509:
      _MC = 0x200, _MP = 0x201, _MW = 0x202, _SC = 0x210, _EC = 0x211, _SP = 0x212, _EP = 0x213;
      GS = (val & 0x80) ? (1 << 15) : 0;
      uint16_t NL;
      NL = ((432 / 8) - 1) << 9;
      if (_lcd_ID == 0x9326 || _lcd_ID == 0x5420)
        NL >>= 1;
      WriteCmdData(0x400, GS | NL);
      goto common_SS;
    default:
      _MC = 0x20, _MP = 0x21, _MW = 0x22, _SC = 0x50, _EC = 0x51, _SP = 0x52, _EP = 0x53;
      GS = (val & 0x80) ? (1 << 15) : 0;
      WriteCmdData(0x60, GS | 0x2700); // Gate Scan Line (0xA700)
    common_SS:
      SS = (val & 0x40) ? (1 << 8) : 0;
      WriteCmdData(0x01, SS); // set Driver Output Control
    common_ORG:
      ORG = (val & 0x20) ? (1 << 3) : 0;
      if (val & 0x08)
        ORG |= 0x1000; // BGR
      _lcd_madctl = ORG | 0x0030;
      WriteCmdData(0x03, _lcd_madctl); // set GRAM write direction and BGR=1.
      break;
    }
  }

  if ((rotation & 1) && ((_lcd_capable & MV_AXIS) == 0))
  {
    uint16_t x;
    x = _MC, _MC = _MP, _MP = x;
    x = _SC, _SC = _SP, _SP = x; //.kbv check 0139
    x = _EC, _EC = _EP, _EP = x; //.kbv check 0139
  }

#ifdef _DEBUG_
  Serial.println("setRotation:_MC=" + String(_MC, HEX));
  Serial.println("setRotation:_MP=" + String(_MP, HEX));
  Serial.println("setRotation:_MW=" + String(_MW, HEX));
  Serial.println("setRotation:_SC=" + String(_SC, HEX));
  Serial.println("setRotation:_SP=" + String(_SP, HEX));
  Serial.println("setRotation:_EC=" + String(_EC, HEX));
  Serial.println("setRotation:_EP=" + String(_EP, HEX));
  Serial.println("setRotation:width=" + String(width()));
  Serial.println("setRotation:height=" + String(height()));
#endif
  setAddrWindow(0, 0, width() - 1, height() - 1);
  vertScroll(0, HEIGHT, 0); // reset scrolling after a rotation
}

void ILI9488p::vertScroll(int16_t top, int16_t scrollines, int16_t offset)
{
  if (_lcd_offset)
  {
    if (rotation == 2 || rotation == 3)
      top += _lcd_offset;
  }
  int16_t bfa = HEIGHT - top - scrollines; // bottom fixed area
  int16_t vsp;
  int16_t sea = top;
  if (_lcd_ID == 0x9327)
    bfa += 32;
  if (offset <= -scrollines || offset >= scrollines)
    offset = 0;       // valid scroll
  vsp = top + offset; // vertical start position
  if (offset < 0)
    vsp += scrollines; // keep in unsigned range
  sea = top + scrollines - 1;
  if (_lcd_capable & MIPI_DCS_REV1)
  {
    uint8_t d[6];    // for multi-byte parameters
    d[0] = top >> 8; // TFA
    d[1] = top;
    d[2] = scrollines >> 8; // VSA
    d[3] = scrollines;
    d[4] = bfa >> 8; // BFA
    d[5] = bfa;
    WriteCmdParamN(is8347 ? 0x0E : 0x33, 6, d);
    //        if (offset == 0 && rotation > 1) vsp = top + scrollines;   //make non-valid
    d[0] = vsp >> 8; // VSP
    d[1] = vsp;
    WriteCmdParamN(is8347 ? 0x14 : 0x37, 2, d);
    if (is8347)
    {
      d[0] = (offset != 0) ? (_lcd_ID == 0x8347 ? 0x02 : 0x08) : 0;
      WriteCmdParamN(_lcd_ID == 0x8347 ? 0x18 : 0x01, 1, d); // HX8347-D
    }
    else if (offset == 0 && (_lcd_capable & MIPI_DCS_REV1))
    {
      WriteCmdParamN(0x13, 0, NULL); // NORMAL i.e. disable scroll
    }
    return;
  }
  else
  {
    //        Serial.println("top=" + String(top) + " vsp=" + String(vsp));
    if (top != 0)
      return;
    // cope with 9320 style variants:
    switch (_lcd_ID)
    {
    case 0x7783:
      WriteCmdData(0x61, _lcd_rev); //! NDL, !VLE, REV
      WriteCmdData(0x6A, vsp);      // VL#
      break;
    case 0x5420:
    case 0x7793:
    case 0x9326:
    case 0xB509:
      WriteCmdData(0x401, (1 << 1) | _lcd_rev); // VLE, REV
      WriteCmdData(0x404, vsp);                 // VL#
      break;
    default:
      // 0x6809, 0x9320, 0x9325, 0x9335, 0xB505 can only scroll whole screen
      WriteCmdData(0x61, (1 << 1) | _lcd_rev); //! NDL, VLE, REV
      WriteCmdData(0x6A, vsp);                 // VL#
      break;
    }
  }
}

void ILI9488p::invertDisplay(boolean i)
{
  uint8_t val;
  _lcd_rev = ((_lcd_capable & REV_SCREEN) != 0) ^ i;
#if _DEBUG_
  Serial.println("invertDisplay=" + String(_lcd_rev, HEX));
#endif

  if (_lcd_capable & MIPI_DCS_REV1)
  {
    if (is8347)
    {
      // HX8347D: 0x36 Panel Characteristic. REV_Panel
      // HX8347A: 0x36 is Display Control 10
      if (_lcd_ID == 0x8347 || _lcd_ID == 0x5252) // HX8347-A, HX5352-A
        val = _lcd_rev ? 6 : 2;                   // INVON id bit#2,  NORON=bit#1
      else
        val = _lcd_rev ? 8 : 10; // HX8347-D, G, I: SCROLLON=bit3, INVON=bit1
      // HX8347: 0x01 Display Mode has diff bit mapping for A, D
      WriteCmdParamN(0x01, 1, &val);
    }
    else
    {
#if _DEBUG_
      Serial.println("invertDisplay 0x21/20");
#endif
      WriteCmdParamN(_lcd_rev ? 0x21 : 0x20, 0, NULL);
    }
    return;
  }
  // cope with 9320 style variants:
  switch (_lcd_ID)
  {
  case 0x9225: // REV is in reg(0x07) like Samsung
  case 0x9226:
  case 0x0154:
    WriteCmdData(0x07, 0x13 | (_lcd_rev << 2)); //.kbv kludge
    break;
  case 0x5420:
  case 0x7793:
  case 0x9326:
  case 0xB509:
    WriteCmdData(0x401, (1 << 1) | _lcd_rev); //.kbv kludge VLE
    break;
  default:
#if _DEBUG_
    Serial.println("invertDisplay 0x61");
#endif
    WriteCmdData(0x61, _lcd_rev);
    break;
  }
}

uint8_t ILI9488p::read8(void)
{
  uint8_t output = 0x00;
  for (int i = 0; i < 8; i++)
  {
    output |= digitalReadFast(_pins[i]) << i;
  }
  return output;
}

uint8_t ILI9488p::read8bits(void)
{
  CS_ACTIVE;
  CD_DATA;
  WR_IDLE;
  RD_ACTIVE;

  // This delay is required for STM32F4 (this is not a STM32F4)
  // delayMicroseconds(1);

  uint8_t result = read8();
  RD_IDLE;
  CS_IDLE;
  return result;
}

uint16_t ILI9488p::read16bits(void)
{
  uint8_t lo;
  uint8_t hi;
  CS_ACTIVE;
  CD_DATA;
  WR_IDLE;
  RD_ACTIVE;
  hi = read8();
  // all MIPI_DCS_REV1 style params are 8-bit
  lo = read8();
  RD_IDLE;
  CS_IDLE;
  return (hi << 8) | lo;
}

uint16_t ILI9488p::readReg16(uint16_t reg)
{
  uint16_t ret;
  setWriteDataBus();
  writeCmdWord(reg);
  setReadDataBus();
  uint8_t hi = read8bits();
  // Serial.print("readReg16 hi=");
  // Serial.println(hi, HEX);

  uint8_t lo = read8bits();
  // Serial.print("readReg16 lo=");
  // Serial.println(lo, HEX);

  setWriteDataBus();
  return (hi << 8) | lo;
}

uint16_t ILI9488p::readReg16Index(uint16_t reg, int8_t index)
{
  uint16_t ret;
  setWriteDataBus();
  writeCmdWord(reg);
  setReadDataBus();
  uint8_t hi;
  uint8_t lo;
  do
  {
    hi = read8bits();
    // Serial.print("readReg16Index hi=");
    // Serial.println(hi, HEX);

    lo = read8bits();
    // Serial.print("readReg16Index lo=");
    // Serial.println(lo, HEX);

  } while (--index >= 0); // need to test with SSD1963
  setWriteDataBus();

  return (hi << 8) | lo;
}

uint32_t ILI9488p::readReg32(uint16_t reg)
{
  uint16_t h = readReg16Index(reg, 0);
  uint16_t l = readReg16Index(reg, 1);
  return ((uint32_t)h << 16) | (l);
}

uint32_t ILI9488p::readReg40(uint16_t reg)
{
  uint16_t h = readReg16Index(reg, 0);
  uint16_t m = readReg16Index(reg, 1);
  uint16_t l = readReg16Index(reg, 2);
  return ((uint32_t)h << 24) | (m << 8) | (l >> 8);
}

uint16_t ILI9488p::readID(void)
{
  uint16_t ret, ret2;
  uint8_t msb;
  ret = readReg16(0); // forces a reset() if called before begin()
  Serial.println("readReg16(0)=0x" + String(ret, HEX));
  if (ret == 0x5408) // the SPFD5408 fails the 0xD3D3 test.
    return 0x5408;
  if (ret == 0x5420) // the SPFD5420 fails the 0xD3D3 test.
    return 0x5420;
  if (ret == 0x8989) // SSD1289 is always 8989
    return 0x1289;
  ret = readReg16(0x67); // HX8347-A
  if (ret == 0x4747)
    return 0x8347;

  ret = readReg32(0xA1); // SSD1963: [01 57 61 01]
  Serial.println("readReg32(A1)=0x" + String(ret, HEX));
  if (ret == 0x6101)
    return 0x1963;
  if (ret == 0xFFFF) // R61526: [xx FF FF FF]
    return 0x1526;   // subsequent begin() enables Command Access
  //    if (ret == 0xFF00)          //R61520: [xx FF FF 00]
  //        return 0x1520;          //subsequent begin() enables Command Access

  ret = readReg40(0xBF);
  Serial.println("readReg40(BF)=0x" + String(ret, HEX));
  if (ret == 0x8357) // HX8357B: [xx 01 62 83 57 FF]
    return 0x8357;
  if (ret == 0x9481) // ILI9481: [xx 02 04 94 81 FF]
    return 0x9481;
  if (ret == 0x1511) //?R61511: [xx 02 04 15 11] not tested yet
    return 0x1511;
  if (ret == 0x1520) //?R61520: [xx 01 22 15 20]
    return 0x1520;
  if (ret == 0x1526) //?R61526: [xx 01 22 15 26]
    return 0x1526;
  if (ret == 0x1581) // R61581:  [xx 01 22 15 81]
    return 0x1581;
  if (ret == 0x1400) //?RM68140:[xx FF 68 14 00] not tested yet
    return 0x6814;
  ret = readReg32(0xD4);
  Serial.println("readReg32(D4)=0x" + String(ret, HEX));
  if (ret == 0x5310) // NT35310: [xx 01 53 10]
    return 0x5310;
  ret = readReg40(0xEF); // ILI9327: [xx 02 04 93 27 FF]
  Serial.println("readReg40(EF)=0x" + String(ret, HEX));
  if (ret == 0x9327)
    return 0x9327;
  ret = readReg32(0xFE) >> 8; // weird unknown from BangGood [04 20 53]
  Serial.println("readReg32(FE)=0x" + String(ret, HEX));
  if (ret == 0x2053)
    return 0x2053;
  uint32_t ret32 = readReg32(0x04);
  Serial.println("readReg32(04)=0x" + String(ret32, HEX));
  msb = ret32 >> 16;
  ret = ret32;
//    if (msb = 0x38 && ret == 0x8000) //unknown [xx 38 80 00] with D3 = 0x1602
#if 0
    if (msb == 0x00 && ret == 0x8000) { //HX8357-D [xx 00 80 00]
        Serial.println("pushComman");
        uint8_t cmds[] = {0xFF, 0x83, 0x57};
        pushCommand(0xB9, cmds, 3);
        msb = readReg(0xD0);
        if (msb == 0x99 || msb == 0x90)
            return 0x9090;      //BIG CHANGE: HX8357-D was 0x8357
    }
#endif
  //    if (msb == 0xFF && ret == 0xFFFF) //R61526 [xx FF FF FF]
  //        return 0x1526;          //subsequent begin() enables Command Access
  if (ret == 0x1526) // R61526 [xx 06 15 26] if I have written NVM
    return 0x1526;   // subsequent begin() enables Command Access
  if (ret == 0x8552) // ST7789V: [xx 85 85 52]
    return 0x7789;
  if (ret == 0xAC11) //?unknown [xx 61 AC 11]
    return 0xAC11;
  //    if (msb == 0x54 && ret == 0x8066)  //RM68140:[xx 54 80 66] But Many different controllers will return same value.
  //        return 0x6814;
  ret = readReg32(0xD3); // for ILI9488, 9486, 9340, 9341
  Serial.println("readReg32(D3)=0x" + String(ret, HEX));
  msb = ret >> 8;
  if (msb == 0x93 || msb == 0x94 || msb == 0x98 || msb == 0x77 || msb == 0x16)
    return ret; // 0x9488, 9486, 9340, 9341, 7796
  if (ret == 0x00D3 || ret == 0xD3D3)
    return ret; // 16-bit write-only bus
  /*
      msb = 0x12;                 //read 3rd,4th byte.  does not work in parallel
      pushCommand(0xD9, &msb, 1);
      ret2 = readReg(0xD3);
      msb = 0x13;
      pushCommand(0xD9, &msb, 1);
      ret = (ret2 << 8) | readReg(0xD3);
  //    if (ret2 == 0x93)
          return ret2;
  */
  ret = readReg16(0); // 0154, 7783, 9320, 9325, 9335, B505, B509
  Serial.println("readReg16(0)=0x" + String(ret, HEX));
  return ret;
}
