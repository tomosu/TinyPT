#pragma once
#include "GeometricProperty.h"
#include "CoordinateTransform.h"
#include <cmath>

#undef _TR1_
#ifdef _TR1_
#include <tr1/random>

class MTengine0to1
{    
 public:
  
  MTengine0to1(){ duration =(double)(mt.max()-mt.min() ); }
  void seed(unsigned int seed_in){ mt.seed(seed_in); }
  double operator()(){ return (double)( mt()-mt.min() ) / duration; }
  
 private:
  double duration;
  std::tr1::mt19937 mt;
};



class UniformRandomGenerator0to1
{
  typedef std::tr1::uniform_real<double> Unif;

 public:
  
  UniformRandomGenerator0to1(unsigned int seed_in){
    eng.seed(seed_in);
    Unif tmp_dist(0, 1.0);
    dist =tmp_dist;
  }
  
  double GenRand(){ return dist(eng); }

 private:
  MTengine0to1 eng;
  Unif dist;
};



#else
#include <cmath>
#include <cstdlib>

class Engine0to1
{    
 public:
  
  Engine0to1(){ duration =(double)RAND_MAX; }
  void seed(unsigned int seed_in){ srand(seed_in); }
  double operator()(){ return (double)rand() / duration; }
  
 private:
  double duration;
};



class UniformRandomGenerator0to1
{

 public:
  
  UniformRandomGenerator0to1(unsigned int seed_in){
    eng.seed(seed_in);
  }
  
  double GenRand(){ return eng(); }

 private:
  Engine0to1 eng;
};

#endif


inline PointVector_t RandomOnUnitSphere(UniformRandomGenerator0to1 &random)
{
  PointVector_t ret;
  double t =random.GenRand() *2.0 *M_PI;
  double f =random.GenRand() *2.0 *M_PI;
  ret.x =sin(t) *cos(f);
  ret.x =sin(t) *sin(f);
  ret.z =cos(t);
  return ret;
}
