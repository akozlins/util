
#include <omp.h>

#include <iostream>

#include <boost/progress.hpp>

const int N = 1000000000;

double noOMP()
{
  boost::progress_timer pt;

  double pi = 0;
  double w = 1.0 / N;
  double ww = 2.0 * w * w;

  double x1 = 1.0 + ww * 0.125;
  double x2 = ww;

  for(int i = 0; i < N; i++)
  {
    x1 += x2;
    x2 += ww;
    pi += 1.0 / x1;
  }

  return pi * w * 4;
}

double withOMP()
{
  boost::progress_timer pt;

  const double w = 1.0 / N;

  double pi = 0;

  #pragma omp parallel
  {
    #pragma omp for reduction(+ : pi)
    for(int i = 0; i < N; i++)
    {
      double x = (0.5 + i) * w;
      pi += 1.0 / (1.0 + x * x);
    }
  }

  return 4.0 * pi * w;
}

void main()
{
  std::cout << noOMP() << std::endl;
  std::cout << withOMP() << std::endl;
}
