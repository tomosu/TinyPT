#pragma once
#include <iostream>

inline void ProgressIndicator(const long long int i, const long long int max)
{
  long long int per=100*i/max;
  std::cout << " " << per << "% ";
}



inline void RunLight(const long long int i, const long long int max)
{
  const int speed =10000;

  int r =i%(speed*4);
  switch(r){
  case speed:
    std::cout << "|";
    ProgressIndicator(i,max);
    std::cout << "\r" << std::flush;
    break;
  case speed*2:
    std::cout << "/";
    ProgressIndicator(i,max);
    std::cout << "\r" << std::flush;
    break;
  case speed*3:
    std::cout << "-";
    ProgressIndicator(i,max);
    std::cout << "\r" << std::flush;
    break;
  case speed*4-1:
    std::cout << "\\";
    ProgressIndicator(i,max);
    std::cout << "\r" << std::flush;
    break;

  default:break;
  }
}
