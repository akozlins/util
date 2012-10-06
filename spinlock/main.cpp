
#include <iostream>

#include <boost/bind.hpp>
#include <boost/thread.hpp>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class Mutex
{
private:
  class ShortLock
  {
  public:
    volatile long locked;

    ShortLock(): locked(0) { }

    void lock() { while(BOOST_INTERLOCKED_COMPARE_EXCHANGE(&locked, 1, 0) == 1); }
    void unlock() { BOOST_INTERLOCKED_EXCHANGE(&locked, 0); }
  };

  ShortLock shortLock;

  HANDLE eventHandle;
public:
  Mutex()
  {
    eventHandle = CreateEvent(0, TRUE, FALSE, 0);
  }

  virtual ~Mutex()
  {
    CloseHandle(eventHandle);
  }

  void lock()
  {
    shortLock.lock();
  }

  void unlock()
  {
    shortLock.unlock();
  }
};

Mutex mt;

void func1()
{
  mt.lock();
  char* str = "Hello World 1 !!!";
  for(int i = 0, len = strlen(str); i < len; i++) std::cout << str[i];
  std::cout << std::endl;
  mt.unlock();
}

void func2()
{
  mt.lock();
  char* str = "Hello World 2 !!!";
  for(int i = 0, len = strlen(str); i < len; i++) std::cout << str[i];
  std::cout << std::endl;
  mt.unlock();
}

void main()
{
  for(int i = 0; i < 10; i++)
  {
    boost::thread thread1(boost::bind(&func1));
    boost::thread thread2(boost::bind(&func2));
    thread2.join();
    thread1.join();
  }
}
