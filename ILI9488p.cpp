#include <ILI9488p.h>

#define MIPI_DCS_REV1 (1 << 0) // using 8 bit command parameters
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

ILI9488p::ILI9488p(void) : Adafruit_GFX(TFTWIDTH, TFTHEIGHT)
{
  pinMode(TFT_RD, OUTPUT);
  pinMode(TFT_WR, OUTPUT);
  pinMode(TFT_DC, OUTPUT);
  pinMode(TFT_CS, OUTPUT);
  pinMode(TFT_RST, OUTPUT);

  CS_IDLE;
  CD_DATA;
  WR_IDLE;
  RD_IDLE;
  RST_IDLE;

  RST_ACTIVE;
  delay(20);
  RST_IDLE;
  delay(150);

  _pins[0] = TFT_D0;
  _pins[1] = TFT_D1;
  _pins[2] = TFT_D2;
  _pins[3] = TFT_D3;
  _pins[4] = TFT_D4;
  _pins[5] = TFT_D5;
  _pins[6] = TFT_D6;
  _pins[7] = TFT_D7;

  _pins[8] = TFT_D8;
  _pins[9] = TFT_D9;
  _pins[10] = TFT_D10;
  _pins[11] = TFT_D11;
  _pins[12] = TFT_D12;
  _pins[13] = TFT_D13;
  _pins[14] = TFT_D14;
  _pins[15] = TFT_D15;
}

void ILI9488p::dataBusWrite(void)
{
  for (int i = 0; i < 16; i++)
    pinMode(_pins[i], OUTPUT);
}

void ILI9488p::dataBusRead(void)
{
  for (int i = 0; i < 16; i++)
    pinMode(_pins[i], INPUT);
}

void ILI9488p::set8(uint8_t bytes)
{
  digitalWriteFast(TFT_D0, (bytes >> 0) & 1);
  digitalWriteFast(TFT_D1, (bytes >> 1) & 1);
  digitalWriteFast(TFT_D2, (bytes >> 2) & 1);
  digitalWriteFast(TFT_D3, (bytes >> 3) & 1);
  digitalWriteFast(TFT_D4, (bytes >> 4) & 1);
  digitalWriteFast(TFT_D5, (bytes >> 5) & 1);
  digitalWriteFast(TFT_D6, (bytes >> 6) & 1);
  digitalWriteFast(TFT_D7, (bytes >> 7) & 1);

  WR_ACTIVE;
  DELAY;
  WR_IDLE;
}

void ILI9488p::set16(uint16_t bytes)
{
  digitalWriteFast(TFT_D0, (bytes >> 0) & 1);
  digitalWriteFast(TFT_D1, (bytes >> 1) & 1);
  digitalWriteFast(TFT_D2, (bytes >> 2) & 1);
  digitalWriteFast(TFT_D3, (bytes >> 3) & 1);
  digitalWriteFast(TFT_D4, (bytes >> 4) & 1);
  digitalWriteFast(TFT_D5, (bytes >> 5) & 1);
  digitalWriteFast(TFT_D6, (bytes >> 6) & 1);
  digitalWriteFast(TFT_D7, (bytes >> 7) & 1);

  digitalWriteFast(TFT_D8, (bytes >> 8) & 1);
  digitalWriteFast(TFT_D9, (bytes >> 9) & 1);
  digitalWriteFast(TFT_D10, (bytes >> 10) & 1);
  digitalWriteFast(TFT_D11, (bytes >> 11) & 1);
  digitalWriteFast(TFT_D12, (bytes >> 12) & 1);
  digitalWriteFast(TFT_D13, (bytes >> 13) & 1);
  digitalWriteFast(TFT_D14, (bytes >> 14) & 1);
  digitalWriteFast(TFT_D15, (bytes >> 15) & 1);

  WR_ACTIVE;
  DELAY;
  WR_IDLE;
}

uint8_t ILI9488p::get8(void)
{
  uint8_t output = 0x00;
  output |= digitalReadFast(TFT_D0) << 0;
  output |= digitalReadFast(TFT_D1) << 1;
  output |= digitalReadFast(TFT_D2) << 2;
  output |= digitalReadFast(TFT_D3) << 3;
  output |= digitalReadFast(TFT_D4) << 4;
  output |= digitalReadFast(TFT_D5) << 5;
  output |= digitalReadFast(TFT_D6) << 6;
  output |= digitalReadFast(TFT_D7) << 7;
  return output;
}

uint16_t ILI9488p::get16(void)
{
  uint16_t output = 0x0000;
  output |= digitalReadFast(TFT_D0) << 0;
  output |= digitalReadFast(TFT_D1) << 1;
  output |= digitalReadFast(TFT_D2) << 2;
  output |= digitalReadFast(TFT_D3) << 3;
  output |= digitalReadFast(TFT_D4) << 4;
  output |= digitalReadFast(TFT_D5) << 5;
  output |= digitalReadFast(TFT_D6) << 6;
  output |= digitalReadFast(TFT_D7) << 7;

  output |= digitalReadFast(TFT_D8) << 8;
  output |= digitalReadFast(TFT_D9) << 9;
  output |= digitalReadFast(TFT_D10) << 10;
  output |= digitalReadFast(TFT_D11) << 11;
  output |= digitalReadFast(TFT_D12) << 12;
  output |= digitalReadFast(TFT_D13) << 13;
  output |= digitalReadFast(TFT_D14) << 14;
  output |= digitalReadFast(TFT_D15) << 15;

  return output;
}

uint8_t ILI9488p::read8bits(void)
{
  CD_DATA;
  CS_ACTIVE;
  WR_IDLE;
  RD_ACTIVE;

  delayNanoseconds(10);
  uint8_t result = get8();
  CS_IDLE;
  RD_IDLE;
  return result;
}

uint16_t ILI9488p::read16bits(void)
{
  uint16_t data;
  CD_DATA;
  CS_ACTIVE;
  WR_IDLE;
  RD_ACTIVE;
  delayNanoseconds(10);
  data = get16();
  CS_IDLE;
  RD_IDLE;
  return data;
}

void ILI9488p::write8(uint8_t c)
{
  CS_ACTIVE;
  CD_DATA;
  RD_IDLE;
  WR_IDLE;

  set8(c);
  WR_ACTIVE;
  DELAY;
  WR_IDLE;
  CS_IDLE;
}

void ILI9488p::write16(uint16_t c)
{
  CS_ACTIVE;
  CD_DATA;
  RD_IDLE;
  WR_IDLE;

  set16(c);
  WR_ACTIVE;
  DELAY;
  WR_IDLE;
  CS_IDLE;
}

void ILI9488p::command(uint8_t cmd)
{
  CS_ACTIVE;
  CD_COMMAND;
  RD_IDLE;
  WR_IDLE;
  set8(cmd);

  CS_IDLE;
}

void ILI9488p::commandP1(uint8_t cmd, uint8_t param)
{
  CS_ACTIVE;
  CD_COMMAND;
  RD_IDLE;
  WR_IDLE;
  set8(cmd);

  CD_DATA;
  set8(param);

  CS_IDLE;
}

void ILI9488p::commandD(uint8_t cmd, uint16_t param)
{
  CS_ACTIVE;
  CD_COMMAND;
  RD_IDLE;
  WR_IDLE;
  set8(cmd);

  CD_DATA;
  set16(param);

  CS_IDLE;
}

void ILI9488p::commandP2(uint8_t cmd, uint8_t param1, uint8_t param2)
{
  CS_ACTIVE;
  CD_COMMAND;
  RD_IDLE;
  WR_IDLE;
  set8(cmd);

  CD_DATA;
  set8(param1);
  set8(param2);

  CS_IDLE;
}

void ILI9488p::commandP4(uint8_t cmd, uint8_t param1, uint8_t param2, uint8_t param3, uint8_t param4)
{
  CS_ACTIVE;
  CD_COMMAND;
  RD_IDLE;
  WR_IDLE;
  set8(cmd);

  CD_DATA;
  set8(param1);
  set8(param2);
  set8(param3);
  set8(param4);

  CS_IDLE;
}

void ILI9488p::commandPN(uint8_t cmd, uint8_t len, uint8_t *param)
{
  CS_ACTIVE;
  CD_COMMAND;
  RD_IDLE;
  WR_IDLE;
  set8(cmd);

  CD_DATA;
  for (uint8_t i = 0; i < len; i++)
    set8(param[i]);

  CS_IDLE;
}

void ILI9488p::commandRD(uint8_t cmd, uint16_t *dest, uint16_t len)
{
  CS_ACTIVE;
  CD_COMMAND;
  RD_IDLE;
  WR_IDLE;
  set8(cmd);

  CD_DATA;
  dataBusRead();
  for (uint8_t i = 0; i < len; i++)
    dest[i] = read16bits();

  dataBusWrite();
  CS_IDLE;
}

void ILI9488p::commandRN(uint8_t cmd, uint8_t *dest, uint16_t len)
{
  CS_ACTIVE;
  CD_COMMAND;
  RD_IDLE;
  WR_IDLE;
  set8(cmd);

  CD_DATA;
  dataBusRead();
  for (uint8_t i = 0; i < len; i++)
    dest[i] = read8bits();

  dataBusWrite();
  CS_IDLE;
}

#define TFTLCD_DELAY 0x7F

void ILI9488p::commandTable(const void *table, int16_t size)
{
  uint8_t *p = (uint8_t *)table, dat[24]; // R61526 has GAMMA[22]
  while (size > 0)
  {
    uint8_t cmd = pgm_read_byte(p++);
    uint8_t len = pgm_read_byte(p++);
    if (cmd == TFTLCD_DELAY)
    {
      delay(len);
      len = 0;
    }
    else
    {
      for (uint8_t i = 0; i < len; i++)
        dat[i] = pgm_read_byte(p++);
      commandPN(cmd, len, dat);
    }
    size -= len + 2;
  }
  if (size != 0)
    Serial.printf("Something wrong with command table...\n");
}

void ILI9488p::setResolution(int16_t width, int16_t height)
{
  _lcd_width = width;
  _lcd_height = height;
}

void ILI9488p::setOffset(int16_t offset)
{
  _lcd_offset = offset;
}

void ILI9488p::begin()
{
  int16_t *p16; // so we can "write" to a const protected variable.
  const uint8_t *table8_ads = NULL;
  int16_t table_size;

  dataBusWrite();

  _lcd_ID = readID();
  _lcd_capable = AUTO_READINC | MIPI_DCS_REV1 | MV_AXIS | READ_BGR;

  static const uint8_t ILI9488cmdTable[] PROGMEM = {
      0xB0, 1, 0x00,                   // unlocks DF to FF
      0xB3, 4, 0x02, 0x00, 0x00, 0x00, // frame memory, interface [02 00 00 00]
      0xB4, 1, 0x00,                   // frame mode
      0xD0, 3, 0x07, 0x42, 0x18,       // power settings [library default] x1.00, x6, x3
      // 0xD0, 3, 0x07, 0x43, 0x15, // power settings [datasheet default] x1.00, x6, x3
      0xD1, 3, 0x00, 0x07, 0x10,             // VCOM control
      0xD2, 2, 0x01, 0x02,                   // power setting for normal mode [library default]
                                             // 0xD2, 2, 0x00, 0x77, // power setting for normal mode [datasheet default]
      0xD3, 2, 0x01, 0x02,                   // power setting for partial mode [library default]
                                             // 0xD3, 2, 0x00, 0x77, // power setting for partial mode [datasheet default]
      0xD4, 2, 0x01, 0x02,                   // power setting for idle mode [library default]
                                             // 0xD4, 2, 0x00, 0x77, // power setting for idle mode [datasheet default]
      0xC0, 5, 0x12, 0x3B, 0x00, 0x02, 0x11, // panel driving settings [library default]
      // 0xC0, 5, 0x00, 0x3B, 0x00, 0x00, 0x11,                                            // panel driving settings [datasheet default]
      0xC1, 3, 0x10, 0x10, 0x88,                                                        // display timing settings for normal mode
      0xC5, 1, 0x03,                                                                    // frame rate [72Hz]
      0xC6, 1, 0x02,                                                                    // interface control
      0xC8, 12, 0x00, 0x32, 0x36, 0x45, 0x06, 0x16, 0x37, 0x75, 0x77, 0x54, 0x0C, 0x00, // gamma settings [library default]
                                                                                        // 0xC8, 12, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // gamma settings [library default]
                                                                                        // 0xCC, 1, 0x00, // does not exist on my datasheet
  };

  if (_lcd_width != 0 || _lcd_height != 0)
  {
    p16 = (int16_t *)&HEIGHT;
    *p16 = _lcd_height;
    p16 = (int16_t *)&WIDTH;
    *p16 = _lcd_width;
  }

  static const uint8_t reset_off[] PROGMEM = {
      0x01, 0,           // soft reset
      TFTLCD_DELAY, 150, // .kbv will power up with ONLY reset, sleep out, display on
      0x3A, 1, 0x55,     // Pixel read=565, write=565.
      0x28, 0,           // Display Off
  };

  static const uint8_t wake_on[] PROGMEM = {
      0x11, 0, // exit sleep mode
      // TFTLCD_DELAY,      150,
      0x29, 0, // Display On
  };

  commandTable(&reset_off, sizeof(reset_off));
  commandTable(ILI9488cmdTable, sizeof(ILI9488cmdTable));
  commandTable(&wake_on, sizeof(wake_on));

  setRotation(0);
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
    commandP4(_MC, x >> 8, x, x >> 8, x);
    commandP4(_MP, y >> 8, y, y >> 8, y);
  }
  else
  {
    commandD(_MC, x);
    commandD(_MP, y);
  }
  commandD(_MW, color);
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
    commandP4(_MC, x >> 8, x, x1 >> 8, x1);
    commandP4(_MP, y >> 8, y, y1 >> 8, y1);
  }
  else
  {
    commandD(_MC, x);
    commandD(_MP, y);
    if (_lcd_capable & XSA_XEA_16BIT)
    {
      if (rotation & 1)
        y1 = y = (y1 << 8) | y;
      else
        x1 = x = (x1 << 8) | x;
    }
    commandD(_SC, x);
    commandD(_SP, y);
    commandD(_EC, x1);
    commandD(_EP, y1);
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
  command(_MW);
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
      write16(color);
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
    command(_MW);
  }
  while (n-- > 0)
  {
    h = (*block++);
    l = (*block++);
    color = h << 8 | l;
    write16(color);
  }
}

void ILI9488p::pushColors(uint16_t *block, int16_t n, bool first)
{
  uint16_t color;
  uint8_t h, l;

  if (first)
    command(_MW);
  while (n-- > 0)
  {
    color = (*block++);
    write16(color);
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
      commandD(_MC, x + col);
      commandD(_MP, y + row);
    }
    command(_MR);
    dataBusRead();
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
      r = get8();
    }
    else
    {
      dummy = read16bits();
    }
    if (_lcd_ID == 0x1511)
      r = get8(); // extra dummy for R61511
    while (n)
    {
      if (_lcd_capable & READ_24BITS)
      {
        r = get8();
        g = get8();
        b = get8();
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
    dataBusWrite();
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
  _MC = 0x2A, _MP = 0x2B, _MW = 0x2C, _SC = 0x2A, _EC = 0x2A, _SP = 0x2B, _EP = 0x2B;
  commandPN(0x36, 1, &val);
  _lcd_madctl = val;

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
    commandPN(is8347 ? 0x0E : 0x33, 6, d);
    //        if (offset == 0 && rotation > 1) vsp = top + scrollines;   //make non-valid
    d[0] = vsp >> 8; // VSP
    d[1] = vsp;
    commandPN(is8347 ? 0x14 : 0x37, 2, d);
    if (is8347)
    {
      d[0] = (offset != 0) ? (_lcd_ID == 0x8347 ? 0x02 : 0x08) : 0;
      commandPN(_lcd_ID == 0x8347 ? 0x18 : 0x01, 1, d); // HX8347-D
    }
    else if (offset == 0 && (_lcd_capable & MIPI_DCS_REV1))
    {
      commandPN(0x13, 0, NULL); // NORMAL i.e. disable scroll
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
      commandD(0x61, _lcd_rev); //! NDL, !VLE, REV
      commandD(0x6A, vsp);      // VL#
      break;
    case 0x5420:
    case 0x7793:
    case 0x9326:
    case 0xB509:
      commandD(0x401, (1 << 1) | _lcd_rev); // VLE, REV
      commandD(0x404, vsp);                 // VL#
      break;
    default:
      // 0x6809, 0x9320, 0x9325, 0x9335, 0xB505 can only scroll whole screen
      commandD(0x61, (1 << 1) | _lcd_rev); //! NDL, VLE, REV
      commandD(0x6A, vsp);                 // VL#
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
      commandPN(0x01, 1, &val);
    }
    else
    {
#if _DEBUG_
      Serial.println("invertDisplay 0x21/20");
#endif
      commandPN(_lcd_rev ? 0x21 : 0x20, 0, NULL);
    }
    return;
  }
  // cope with 9320 style variants:
  switch (_lcd_ID)
  {
  case 0x9225: // REV is in reg(0x07) like Samsung
  case 0x9226:
  case 0x0154:
    commandD(0x07, 0x13 | (_lcd_rev << 2)); //.kbv kludge
    break;
  case 0x5420:
  case 0x7793:
  case 0x9326:
  case 0xB509:
    commandD(0x401, (1 << 1) | _lcd_rev); //.kbv kludge VLE
    break;
  default:
#if _DEBUG_
    Serial.println("invertDisplay 0x61");
#endif
    commandD(0x61, _lcd_rev);
    break;
  }
}

uint16_t ILI9488p::readID(void)
{
  // manual 5.2.3 Read Display ID (04)
  uint8_t ret04[4];
  commandRN(0x04, ret04, 4);
  Serial.printf("LCD module manufacturer ID: %02x\n", ret04[1]);
  Serial.printf("LCD module/driver version ID: %02x\n", ret04[2]);
  Serial.printf("LCD module/driver ID: %02x\n", ret04[3]);

  // manual 5.3.30 Read ID4 (D3)
  uint8_t retD3[4];
  commandRN(0xD3, retD3, 4);
  uint16_t ID = ((uint16_t)retD3[2] << 8) + retD3[3];
  Serial.printf("ID: %04x\n", ID);

  return ID;
}
