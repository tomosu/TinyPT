#include "Photon.h"
#include "GeometricProperty.h"
#include "PlotUtil.h"
#include "RunLightUtil.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <numeric>


void CalcCoherentScattering(const long long int iter_num,
			    const double energy_MeV,
			    SharedCompoundPtr target)
{

  // Gen X axis
  int graduation=50;
  double radStep=180.0/(double)graduation;
  std::vector<double> rad;
  double sum=0.0;
  for(int i=0; i<graduation; i++){ rad.push_back(sum); sum+=radStep; }

  // Gen Y axis
  UniformRandomGenerator0to1 random(12345);
  std::vector<double> hist(graduation, 0);
  for(long long int i=0; i<iter_num; i++){
    RunLight(i, iter_num);
    PointVector_t ray(1.0, 1.0, 1.0);
    SharedPhotonPtr photon = Photon::CreatePhoton(energy_MeV, ray);
    while(photon->CoherentScattering(target, random) == MC_REJECTED){}
    for(int j=0; j<graduation; j++){
      if(photon->GetIncline_rad() < (double)(j *radStep)*M_PI/180.0 ){ hist[j]++; break;}
    }
  }
  double sumhist =std::accumulate(hist.begin(), hist.end(), 0.0);
  for(int i=0; i<hist.size(); i++){ hist[i] /=sumhist;}

  // Plot
  std::cout << "\r" << "     ";
  std::string mark("*");
  std::string xlabel("degree");
  std::string ylabel("probability");
  Plotter plotter(graduation, 18, xlabel, ylabel);
  plotter.Plot(rad, hist, mark);

}




void CalcIncoherentScattering(const long long int iter_num,
			      const double energy_MeV,
			      SharedCompoundPtr target)
{

  // Gen X axis
  int graduation=50;
  double radStep=180.0/(double)graduation;

  std::vector<double> rad;
  double sum=0.0;
  for(int i=0; i<graduation; i++){ rad.push_back(sum); sum+=radStep; }

  // Gen Y axis
  UniformRandomGenerator0to1 random(12345);
  std::vector<double> hist(graduation, 0);
  for(long long int i=0; i<iter_num; i++){
    RunLight(i,iter_num);
    PointVector_t ray(1.0, 1.0, 1.0);
    SharedPhotonPtr photon = Photon::CreatePhoton(energy_MeV, ray);
    while(photon->IncoherentScattering(target, random) == MC_REJECTED){}
    for(int j=0; j<graduation; j++){
      if(photon->GetIncline_rad() < (double)(j *radStep)*M_PI/180.0 ){ hist[j]++; break;}
    }
  }
  double sumhist =std::accumulate(hist.begin(), hist.end(), 0.0);
  for(int i=0; i<hist.size(); i++){ hist[i] /=sumhist;}

  // Plot
  std::cout << "\r" << "     ";
  std::string mark("*");
  std::string xlabel("degree");
  std::string ylabel("probability");
  Plotter plotter(graduation, 18, xlabel, ylabel);
  plotter.Plot(rad, hist, mark);
}



MaterialDataFiles_t SetMaterialFiles()
{
  MaterialDataFiles_t material_files;
  std::string elementDir("../../Data/MaterialData/PhotonCrossSection");
  std::string coh("../../Data/MaterialData/XrayFormFactor/pgs5form_X100_Z100.dat");
  std::string incoh("../../Data/MaterialData/XrayFormFactor/incoh_X45_Z100.dat");
  std::string k_alpha("../../Data/MaterialData/CharacteristicXray/CharacteristicXrayK_alpha_Z100.dat");
  
  material_files.PhotonCrossSectionDir = elementDir;
  material_files.CoherentFilepath = coh;
  material_files.IncoherentFilepath = incoh;
  material_files.K_alphaFilepath = k_alpha;

  return material_files;
}



int main(){
  
  //////////////////////////
  // Create target material
  //////////////////////////
  MaterialDataFiles_t mat_files=SetMaterialFiles();
  std::string compoundJson("../../Data/MaterialData/ChemicalCompound/ChemicalCompound.json");
  std::string compoundName("CsI");
  SharedCompoundPtr target = Compound::CreateCompound(compoundJson, compoundName, mat_files);
  
  target->PrintCompoundInfo();


  ////////////////////////
  // Photon setup
  ////////////////////////
  double energy_MeV=0.04;
  long long int iter_num=1000000;

  std::cout << "Calc Rayleigh scattering angle distribution @" << energy_MeV << "MeV" <<std::endl;
  CalcCoherentScattering(iter_num, energy_MeV, target);
  std::cout << std::endl;
  
  std::cout << "Calc Compton scattering angle distribution @" << energy_MeV << "MeV" <<std::endl;
  CalcIncoherentScattering(iter_num, energy_MeV, target);
  std::cout << std::endl;

  return 0;
}
