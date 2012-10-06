#if !defined(__Graphics_h)
#define __Graphics_h

#include "../driver/Driver.h"

#if defined(GRAPHICS_EXPORTS)
#define GRAPHICS_DECL __declspec(dllexport)
#else
#define GRAPHICS_DECL __declspec(dllimport)
#endif

#include <boost/smart_ptr.hpp>

class GRAPHICS_DECL Graphics: public boost::enable_shared_from_this<Graphics>
{
public:
  virtual ~Graphics()
  {
  }

  virtual void clear() = 0;
  virtual void flush() = 0;

  virtual void drawPixel(__int32 x, __int32 y) = 0;
  virtual void drawLine(__int32 x1, __int32 y1, __int32 x2, __int32 y2) = 0;
  virtual void drawRect(__int32 x1, __int32 y1, __int32 x2, __int32 y2) = 0;
  virtual void fillRect(__int32 x1, __int32 y1, __int32 x2, __int32 y2) = 0;
  virtual void drawString(const wchar_t* str, __int32 x, __int32 y) = 0;
};

typedef boost::shared_ptr<Graphics> GraphicsPtr;

class GRAPHICS_DECL Font
{
public:
  virtual ~Font()
  {
  }

  virtual void drawString(const GraphicsPtr& graphics, const wchar_t* str, __int32 x, __int32 y) = 0;
};

typedef boost::shared_ptr<Font> FontPtr;

class GRAPHICS_DECL GraphicsG1S: public Graphics
{
private:
  DriverPtr driver;

  boost::scoped_array<unsigned __int8> buffer;

  FontPtr font;
public:
  GraphicsG1S();

  void clear();
  void flush();

  void drawPixel(__int32 x, __int32 y);
  void drawLine(__int32 x1, __int32 y1, __int32 x2, __int32 y2);
  void drawRect(__int32 x1, __int32 y1, __int32 x2, __int32 y2);
  void fillRect(__int32 x1, __int32 y1, __int32 x2, __int32 y2);
  void drawString(const wchar_t* str, __int32 x, __int32 y);
};

#endif // __Graphics_h
