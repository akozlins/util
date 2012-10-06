#if !defined(__Driver_h)
#define __Driver_h

#if defined(DRIVER_EXPORTS)
  #define DRIVER_DECL __declspec(dllexport)
#else
  #define DRIVER_DECL __declspec(dllimport)
#endif

#include <boost/smart_ptr.hpp>

class DRIVER_DECL Driver
{
public:
  virtual ~Driver()
  {
  }

  virtual unsigned __int32 getWidth() const = 0;
  virtual unsigned __int32 getHeight() const = 0;

  virtual void init() = 0;
  virtual void flush() = 0;
  virtual void write(const unsigned __int8* buffer, unsigned __int32 length) = 0;
};

typedef boost::shared_ptr<Driver> DriverPtr;

class DRIVER_DECL DriverG1S: public Driver
{
private:
  void* handle;

  unsigned __int8* frame1;
  unsigned __int8* frame2;
public:
  DriverG1S();
  ~DriverG1S();

  unsigned __int32 getWidth() const;
  unsigned __int32 getHeight() const;

  void init();
  void flush();
  void write(const unsigned __int8* buffer, unsigned __int32 length);
};

#endif // __Driver_h
