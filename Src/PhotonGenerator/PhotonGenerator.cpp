#include "PhotonGenerator.h"
#include "StringUtil.h"
#include <algorithm>


///////////////////////////
//
// Base Class
//
///////////////////////////
SharedPhotonGeneratorPtr PhotonGenerator::Create(const PhotonGeneratorParameter_t &param)
{
  
  if(param.Type == "ConeBeam"){
    SharedPhotonGeneratorPtr ret(new ConeBeamGenerator(param));
    return ret;

  }else{
    assert(!"Invalid Generatror Type");
  }
};


SharedFlyingPhotonPtr PhotonGenerator::GeneratePhoton(UniformRandomGenerator0to1 &)
{
}




///////////////////////////
//
// Cone Beam Generator
//
///////////////////////////
class PredicateGreaterEnergyDistribution
{
public:
  inline bool operator()(const EnergyDistribution_t &lhs, const EnergyDistribution_t &rhs){
    return lhs.Energy_MeV < rhs.Energy_MeV;
  }
};



bool ReadEnergyDistributionCSV(const std::string &filepath,
			       std::vector<EnergyDistribution_t> &EnergyDistribution,
			       std::vector<EnergyDistribution_t> &CumulativeEnergyDistribution)
{
  // Open file
  std::ifstream ifs(filepath.c_str());
  if(ifs.fail()){
    std::cout << "file not found:" << filepath << std::endl;
    return false;
  }  
  

  // Read all data from csv
  std::string str;
  std::string delim(",");
  std::vector<double> data;
  
  while( getline(ifs, str) ){
    std::vector<std::string> token;
    token = my_split(str, delim);
    for(int i=0; i<token.size(); i++){
      data.push_back( my_stod(token[i]) );
    }
  }

  
  // Generate energy distribution
  for(int i=0; i< data.size()/2; i++){
    EnergyDistribution_t token;
    token.Energy_MeV =data[2*i];
    token.Probability =data[2*i+1];
    EnergyDistribution.push_back(token);
  }
  
  std::sort(EnergyDistribution.begin(), EnergyDistribution.end(), PredicateGreaterEnergyDistribution());

  // Gen cumulative
  double probSum=0.0;
  for(int i=0; i<EnergyDistribution.size(); i++){
    probSum += EnergyDistribution[i].Probability;
    EnergyDistribution_t ced;
    ced.Probability =probSum;
    ced.Energy_MeV =EnergyDistribution[i].Energy_MeV;
    CumulativeEnergyDistribution.push_back(ced);
  }

  return true;
}




/////////////////////
// Member Function
////////////////////
ConeBeamGenerator::ConeBeamGenerator(const PhotonGeneratorParameter_t &param)
  : Direction(param.Direction), ConeAngle(param.ConeAngle), Position(param.Position)
{
  this->CosMin =cos(ConeAngle/2.0);
  ReadEnergyDistributionCSV(param.EnergyDistributionFilepath,
			    this->EnergyDistribution,
			    this->CumulativeEnergyDistribution);
};



SharedFlyingPhotonPtr ConeBeamGenerator::GeneratePhoton(UniformRandomGenerator0to1 &random)
{
  
  return FlyingPhoton::CreateFlyingPhoton( this->GenerateRandomEnergy(random),
					   this->GenerateRandomDirection(random),
					   this->Position);
};



PointVector_t ConeBeamGenerator::GenerateRandomDirection(UniformRandomGenerator0to1 &random)
{
  SphericalCoordinateAngle_t displace;
  if(this->ConeAngle > 0.0){
    double cosTheta = (1.0 -this->CosMin) *random.GenRand() + this->CosMin;
      displace.Inclination = acos(cosTheta);
      displace.Azimuth = 2.0 *M_PI *random.GenRand();
    }else{
      return this->Direction;
    }
  return ChangeDirectionBySphericalCoordinateAngle(this->Direction, displace);
}



class PredicateGreaterProbability
{
public:
  PredicateGreaterProbability(double val_in) : Val(val_in){};
  double Val;
  inline bool operator()(EnergyDistribution_t &in){ return this->Val < in.Probability;}
};



double ConeBeamGenerator::GenerateRandomEnergy(UniformRandomGenerator0to1 &random)
{
  PredicateGreaterProbability pred( random.GenRand() );
  std::vector<EnergyDistribution_t>::iterator it;
  it =std::find_if(this->CumulativeEnergyDistribution.begin(), this->CumulativeEnergyDistribution.end(), pred);
  return it->Energy_MeV;
}
