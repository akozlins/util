
#include "Graphics.h"

#include <string>

class FontImpl: public Font
{
private:
  std::wstring chars;

  unsigned __int32 imageWidth;
  unsigned __int32 imageHeight;

  boost::scoped_array<unsigned __int8> image;

  unsigned __int32 imageOffset;

  boost::scoped_array<unsigned __int32> widths;
  boost::scoped_array<unsigned __int32> advances;
  boost::scoped_array<unsigned __int32> origins;
  boost::scoped_array<unsigned __int32> offsets;
public:
  FontImpl()
  {
    chars = L"0123456789%: ";

    imageWidth = 57;
    imageHeight = 7;

    static const unsigned __int8 image_[7][57] = {
      0, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0,
      1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0,
      1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1,
      1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0,
      1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 1, 0,
      1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0,
      0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1
    };

    image.reset(new unsigned __int8[imageWidth * imageHeight]); memcpy(image.get(), image_, imageWidth * imageHeight);

    imageOffset = 3;

    static const unsigned __int32 widths_[13] = { 5, 3, 5, 5, 6, 5, 5, 5, 5, 5, 7, 1, 0 };
    static const unsigned __int32 advances_[13] = { 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 9, 3, 3 };
    static const unsigned __int32 origins_[13] = { 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0 };
    static const unsigned __int32 offsets_[13] = { 0, 5, 8, 13, 18, 24, 29, 34, 39, 44, 49, 56, 57 };

    widths.reset(new unsigned __int32[13]); memcpy(widths.get(), widths_, 13 << 2);
    advances.reset(new unsigned __int32[13]); memcpy(advances.get(), advances_, 13 << 2);
    origins.reset(new unsigned __int32[13]); memcpy(origins.get(), origins_, 13 << 2);
    offsets.reset(new unsigned __int32[13]); memcpy(offsets.get(), offsets_, 13 << 2);
  }

  void drawString(const GraphicsPtr& graphics, const wchar_t* str, __int32 x, __int32 y)
  {
    y += imageOffset;

    for(unsigned __int32 off = 0, len = wcslen(str); off < len; ++off)
    {
      int j = chars.find(str[off]);
      if(j == -1) continue;

      unsigned __int32 width = widths[j];
      unsigned __int32 origin = origins[j];
      unsigned __int32 offset = offsets[j];

      for(unsigned __int32 cy = 0; cy < imageHeight; cy++, offset += imageWidth) for(unsigned __int32 cx = 0; cx < width; cx++)
      {
        if(image[offset + cx] != 0) graphics->drawPixel(x + origin + cx, y + cy);
      }

      x += advances[j];
    }
  }
};

GraphicsG1S::GraphicsG1S()
{
  buffer.reset(new unsigned __int8[4096]);

  font.reset(new FontImpl());

  driver.reset(new DriverG1S());
  driver->init();
}

void GraphicsG1S::clear()
{
  memset(buffer.get(), 0, 4096);
}

void GraphicsG1S::flush()
{
  driver->write(buffer.get(), 4096);
  driver->flush();
}

void GraphicsG1S::drawPixel(__int32 x, __int32 y)
{
  if(x >= 0 && x < 128 && y >= 0 && y < 32) buffer[x + (y << 7)] = 1;
}

void GraphicsG1S::drawLine(__int32 x1, __int32 y1, __int32 x2, __int32 y2)
{
  __int32 dx = x2 - x1;
  __int32 dy = y2 - y1;

  double m, b;

  drawPixel(x1, y1);
  if(abs(dx) > abs(dy))
  {
    m = (double) dy / (double) dx;
    b = y1 - m*x1;
    dx = (dx < 0) ? -1 : 1;
    while (x1 != x2)
    {
      x1 += dx;
      y1 = (__int32)(m * x1 + b + 0.5);
      drawPixel(x1, y1);
    }
  }
  else if (dy != 0)
  {
    m = (double) dx / (double) dy;
    b = x1 - m*y1;
    dy = (dy < 0) ? -1 : 1;
    while (y1 != y2)
    {
      y1 += dy;
      x1 = (__int32)(m * y1 + b + 0.5);
      drawPixel(x1, y1);
    }
  }
}

void GraphicsG1S::drawRect(__int32 x1, __int32 y1, __int32 x2, __int32 y2)
{
  drawLine(x1, y1, x1, y2);
  drawLine(x2, y1, x2, y2);
  drawLine(x1, y1, x2, y1);
  drawLine(x1, y2, x2, y2);
}

void GraphicsG1S::fillRect(__int32 x1, __int32 y1, __int32 x2, __int32 y2)
{
  for(int x = x1; x <= x2; x++)
  {
    drawLine(x, y1, x, y2);
  }
}

void GraphicsG1S::drawString(const wchar_t* str, __int32 x, __int32 y)
{
  FontPtr font_ = font;
  if(font_) font_->drawString(shared_from_this(), str, x, y);
}
