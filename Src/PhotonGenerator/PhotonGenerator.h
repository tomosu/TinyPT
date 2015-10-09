#pragma once
#include "FlyingPhoton.h"
#include "GeometricProperty.h"
#include "RandomNumberGenerator.h"
#include <cmath>
#include <string>
#include <vector>
#include <cassert>

class PhotonGenerator;
typedef std::tr1::shared_ptr<PhotonGenerator> SharedPhotonGeneratorPtr;


struct EnergyDistribution_t
{
  double Probability; // Relative or Cumulative
  double Energy_MeV;
};

  
struct PhotonGeneratorParameter_t
{
  std::string Type;
  std::string EnergyDistributionFilepath;
  PointVector_t Position;
  PointVector_t Direction;
  double ConeAngle;
};





/////////////////////////
//
// Interface
//
/////////////////////////
class PhotonGenerator
{
 public:
  
  static SharedPhotonGeneratorPtr Create(const PhotonGeneratorParameter_t&);
  virtual SharedFlyingPhotonPtr GeneratePhoton(UniformRandomGenerator0to1 &);
};




////////////////////////
//
// Cone Beam Generator
//
////////////////////////


class ConeBeamGenerator : public PhotonGenerator
{
 public:
  
  ConeBeamGenerator(const PhotonGeneratorParameter_t &);
  SharedFlyingPhotonPtr GeneratePhoton(UniformRandomGenerator0to1 &random);

  
 private:
  double GenerateRandomEnergy(UniformRandomGenerator0to1 &random);
  PointVector_t GenerateRandomDirection(UniformRandomGenerator0to1 &random);
  
  std::vector<EnergyDistribution_t> EnergyDistribution;
  std::vector<EnergyDistribution_t> CumulativeEnergyDistribution;
  PointVector_t Position;
  PointVector_t Direction;
  double ConeAngle;
  double CosMin;
};
