
#include <string>

#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>

class System
{
private:
  void* locator;
  void* services;

  unsigned __int32 processorUsage[3];

  boost::mutex mt;
  boost::scoped_ptr<boost::thread> thr;
public:
  System();
  ~System();

  std::wstring getTime() const;

  unsigned __int32 getBatteryLife() const;

  unsigned __int32 getProcessorUsage(unsigned __int32 n) const;
private:
  void run();
  void update(__int64 tp[3], __int64 ts[3]);
};
