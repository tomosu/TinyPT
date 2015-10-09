#include <cmath>
#include <iostream>
#include "FlyingPhoton.h"
#include "Detector.h"
#include "CoordinateTransform.h"
#include "GeometricProperty.h"
#include "PhotonGenerator.h"
#include "Quaternion.h"
#include "StringUtil.h"
#include "RunLightUtil.h"




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


void RotateParam(const Rotor &rot, DetectorParameter_t &detParam, PhotonGeneratorParameter_t &pgParam)
{
  //Detector rotation
  detParam.NormalVector =rot.Rotate(detParam.NormalVector);
  detParam.UpVector =rot.Rotate(detParam.UpVector);
  detParam.Center =rot.Rotate(detParam.Center);

  //Source rotation
  pgParam.Position =rot.Rotate(pgParam.Position);
  pgParam.Direction =rot.Rotate(pgParam.Direction); 
}



//////////////////////////////////
//
// Cone beam CT simulator example
//
//////////////////////////////////

int main()
{

  ////////////////////////////////
  // Phantom param set up
  ////////////////////////////////
  std::string compoundJsonPath("./ChemicalCompound.json");
  std::string phantomJsonPath("./TestPhantom.json");
  MaterialDataFiles_t material_files =SetMaterialFiles();
  

  ////////////////////////////////
  // Detector param set up
  ////////////////////////////////
  PointVector_t normal(0.0, 0.0, 1.0);
  PointVector_t up(0.0, 1.0, 0.0);
  PointVector_t center(0.0, 0.0, -12.0);

  DetectorParameter_t detParam;
  detParam.Type="PlaneIdealDetector";
  detParam.NormalVector =normal;
  detParam.UpVector =up;
  detParam.Center =center;
  detParam.PitchU=0.2;
  detParam.PitchV=0.2;
  detParam.SizeU=128;
  detParam.SizeV=128;
  
 
  ////////////////////////////////
  // Photon generator param set up
  ////////////////////////////////
  PointVector_t direction(0.0, 0.0, -1.0);
  PointVector_t position(0.0, 0.0, 20.0);
  
  PhotonGeneratorParameter_t pgParam;
  pgParam.Type ="ConeBeam";
  pgParam.EnergyDistributionFilepath ="PhotonEnergyDistribution.csv";
  pgParam.Position =position;
  pgParam.Direction =direction;
  pgParam.ConeAngle =M_PI/2.5;
  

  ////////////////////////////////
  // Geometry rotor set up
  ////////////////////////////////
  int projectionNum=8;
  PointVector_t gantryAxis(0.0, 1.0, 0.0);
  double radian =2.0*M_PI/(double)projectionNum;

  Rotor rot(gantryAxis, radian);
  


  ////////////////////////////////
  // CT simulation
  ////////////////////////////////
  long long int simCount =1000000;
  UniformRandomGenerator0to1 random(12345);

  for(int proj=0; proj<projectionNum; proj++){
    std::cout << "projection " << proj << std::endl;

    //create simulator
    SharedPhantomTreePtr ptree = PhantomTree::Create(phantomJsonPath, compoundJsonPath, material_files);
    SharedDetectorPtr detector =Detector::Create(detParam);
    SharedPhotonGeneratorPtr pg =PhotonGenerator::Create(pgParam);

    //montecarlo sim phantom
    for(long long int i=0; i<simCount; i++){

      RunLight(i, simCount);
      SharedFlyingPhotonPtr photon =pg->GeneratePhoton(random);
      ptree->TransportPhotonThroughPhantomTree(random, photon); 
      if( !photon->IsAbsorbed()){
	detector->Detect(photon);
      }
    }

    //geometry rotation
    RotateParam(rot, detParam, pgParam);

    //export result
    std::string outpath;
    std::stringstream ss;
    ss << "projection_"<< detParam.SizeU << "x" << detParam.SizeV << "_" << proj << ".i32"; 
    ss >> outpath;

    double gain=10000.0;
    detector->Export(outpath, gain);
  }

  return 0;
}

