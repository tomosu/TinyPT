#pragma once
#include <tr1/memory>
#include <vector>
#include <string>
#include <iostream>


struct Element;
typedef std::tr1::shared_ptr<Element> SharedElementPtr;


class Compound;
typedef std::tr1::shared_ptr<Compound> SharedCompoundPtr;


/////////////////////
//
// File definition
//
/////////////////////
struct MaterialDataFiles_t{
  std::string PhotonCrossSectionDir;
  std::string CoherentFilepath;
  std::string IncoherentFilepath;
  std::string K_alphaFilepath;
};



/////////////////////
//
// Element
//
/////////////////////
struct AtomicFormFactor_t
{

  // Photon wave length in angstroms :lambda 
  // Photon scattering angle :t
  // Momentum transfer: x =sin(t/2)/lambda.
  
  double MomentumTransfer_one_per_angstrom;
  
  // atomic form factor
  double F;

};


struct PhotonCrossSection_t
{

  double Energy_MeV; //
  
  double PhotoElectricCrossSection_cm2_per_gram;
  double CoherentScatteringCrossSection_cm2_per_gram;
  double IncoherentScatteringCrossSection_cm2_per_gram;
  //double PairProductionCrossSection_cm2_per_gram;

  double MassAbsorptionCoefficient_cm2_per_gram;
  
};



//////////////////
// Element Class
//////////////////
class Element{

public:

  //Factory
  static SharedElementPtr CreateElement(const MaterialDataFiles_t &,
					const int);


  //Print
  void PrintElementInfo();
  
  //Coherent(Rayleigh)
  int CoherentScatteringFormFactorSize(){ return this->CoherentScatteringFormFactor.size(); };

  double GetCoherentMomentumTransferByIndex(const int idx){
    return this->CoherentScatteringFormFactor[idx].MomentumTransfer_one_per_angstrom;
  };

  double GetCoherentFormFactorByIndex(const int idx){
    return this->CoherentScatteringFormFactor[idx].F;
  };


  //Incoherent(Compton)
  int IncoherentScatteringFormFactorSize(){ return this->IncoherentScatteringFormFactor.size(); };

  double GetIncoherentMomentumTransferByIndex(const int idx){
    return this->IncoherentScatteringFormFactor[idx].MomentumTransfer_one_per_angstrom;
  };

  double GetIncoherentFormFactorByIndex(const int idx){
    return this->IncoherentScatteringFormFactor[idx].F;
  };


  //Photon cross section
  PhotonCrossSection_t GetPhotonCrossSectionByEnrgy(const double);


  //K_alpha
  double GetK_alpha_MeV(){ return this->K_alpha_MeV;}

  //Atomic Weight
  double GetAtomicWeight(){ return this->AtomicWeight;}
  
  //Atomic Number
  int GetAtomicNumber(){ return this->Z;}

private:
  void GenerateAtomicFormFactor(const std::string&, //table path
				const int, //atomic number
				std::vector<AtomicFormFactor_t> &); //form factor

  void GeneratePhotonCrossSection(const std::string&, //table dir path
				  const int, // atomic number
				  std::vector<PhotonCrossSection_t> & ); //cross section

  // Read table and convert unit. [angstroms] -> [MeV]
  void GenerateK_alpha(const std::string &, //table path
		       const int); // atomic number

  
  int Z; //atomic number

  //Initiliazed in GenerateK_alpha
  double K_alpha_MeV; //characteristic xray

  //Initialized in GeneratePhotonCrossSection
  std::string ElementName;
  double Density_gram_per_cm3;
  double AtomicWeight; //relative atomic mass
  std::vector<PhotonCrossSection_t> PhotonCrossSection;

  //Initialized in GenerateAtomicFormFactor
  std::vector<AtomicFormFactor_t> CoherentScatteringFormFactor;
  std::vector<AtomicFormFactor_t> IncoherentScatteringFormFactor;

};




////////////////////
//
// Compound
//
////////////////////
struct ChemicalComposition_t
{
  // the number of atom(Z) in a chemical compound
  int Quantity;

  // pointer to element
  SharedElementPtr Element;
};


struct K_alphaInfo_t
{
  double K_alpha_MeV;
  double RelativeProbability;
  int Z;
};

typedef std::vector<K_alphaInfo_t> ComposedK_alpha;





////////////////////
// Compound Class
////////////////////
class Compound{


public:

  //Factory
  static SharedCompoundPtr CreateCompound(const std::string &jsonPath,
					  const std::string &CompoundNameToGenerate,
					  const MaterialDataFiles_t &dataFiles);

  //General
  std::string GetCompoundName(){ return this->CompoundName; };
  double GetDensity_gram_per_cm3(){ return this->Density_gram_per_cm3; };

  //Print info
  void PrintCompoundInfo();
  void PrintComposedCoherentScatteringFormFactor();
  void PrintComposedCoherentScatteringFormFactorDistribution();
  void PrintComposedIncoherentScatteringFormFactor();

  //Photon cross section & mass absoption
  PhotonCrossSection_t GetComposedPhotonCrossSectionByEnergy(const double energy_MeV);

  // Coherent(Rayleigh)
  double GetComposedCoherentScatteringFormFactorByMomentumTransfer(const double x_one_per_angstrom);
  double GetComposedCoherentScatteringFormFactorDistributionByMomentumTransfer(const double x_one_per_angstrom);
  double GetComposedCoherentScatteringFormFactorDistributionMax() const 
  {
    return ComposedCoherentScatteringFormFactorDistributionMax;
  };
  double GetMomentumTransferByCoposedCoherentScatteringFormFactorDistribution(const double F);


  // Incoherent(Compton)
  double GetComposedIncoherentScatteringFormFactorByMomentumTransfer(const double x_one_per_angstrom);
  double GetComposedIncoherentScatteringFormFactorMax() const
  {
    return ComposedIncoherentScatteringFormFactorMax;
  };


  //K_alpha distribution
  ComposedK_alpha GetComposedK_alphaByEnergy(const double energy_MeV);
  

private:

  // Coherent(Rayleigh)
  void GenerateComposedCoherentScatteringFormFactor(std::vector<ChemicalComposition_t> &);
  void CalcComposedCoherentScatteringFormFactorDistribution();

  // Incoherent(Compton)
  void GenerateComposedIncoherentScatteringFormFactor(std::vector<ChemicalComposition_t> &);  
  
  std::string CompoundName;
  double Density_gram_per_cm3;

  std::vector<ChemicalComposition_t> ChemicalComposition;
  
  //Coherent(Rayleigh)
  std::vector<AtomicFormFactor_t> ComposedCoherentScatteringFormFactor;
  std::vector<AtomicFormFactor_t> ComposedCoherentScatteringFormFactorDistribution;
  double ComposedCoherentScatteringFormFactorDistributionMax;
  

  //Incoherent(Compton)
  std::vector<AtomicFormFactor_t> ComposedIncoherentScatteringFormFactor;
  double ComposedIncoherentScatteringFormFactorMax;  

};
