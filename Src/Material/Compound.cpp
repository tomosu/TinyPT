#include "picojson.h"
#include "Compound.h"
#include "StringUtil.h"
#include "MiscCalculation.h"
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>


///////////////////////////
//
// Element
//
///////////////////////////
void Element::GenerateAtomicFormFactor(const std::string &filepath,
				       const int Z, //atomic number
				       std::vector<AtomicFormFactor_t>&out)
{

  /////////////////////
  // Open file
  /////////////////////
  std::ifstream ifs(filepath.c_str());
  if(ifs.fail()){
    std::cout << "file not found:" << filepath << std::endl;
  }
  
  /////////////////////
  // Read all data from csv
  /////////////////////
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

  /////////////////////
  // Generate atomic form factor
  /////////////////////  
  int tableNum = (int) data[0];
  for(int i=0; i< tableNum; i++){

    int ofst=1;

    AtomicFormFactor_t token;
    token.MomentumTransfer_one_per_angstrom = data[i+ofst];
    token.F = data[Z*tableNum +i +ofst];

    out.push_back(token);
  }
  
}




void Element::GeneratePhotonCrossSection(const std::string &dirpath,
					 const int Z, //atomic number
					 std::vector<PhotonCrossSection_t> &out)
{
  
  /////////////////////
  // Open file
  /////////////////////
  char filepath[512];
  sprintf(filepath, "%s/%02d.ele", dirpath.c_str(), Z);

  std::ifstream ifs(filepath);
  if(ifs.fail()){
    std::cout << "file not found:" << filepath << std::endl;
  }
  
  
  /////////////////////
  // Read all data from csv
  /////////////////////
  std::string str;
  std::string delim(",");
  std::vector<std::string> stringData;
  
  while( getline(ifs, str) ){
    std::vector<std::string> token;
    token = my_split(str, delim);
    for(int i=0; i<token.size(); i++){
      stringData.push_back(token[i]);
    }
  }

  
  /////////////////////////
  // Initialize private member
  /////////////////////////
  this->ElementName.swap(stringData[0]);
  this->Density_gram_per_cm3 = my_stod(stringData[1]);
  this->AtomicWeight = my_stod( stringData.back() );

  
  /////////////////////////
  // Read table
  /////////////////////////
  int tableNum =( my_stoi(stringData[2]));
  for(int i=0; i<tableNum; i++){
    PhotonCrossSection_t token;
    int ofst=3;
    int col_num=6;
    
    token.Energy_MeV = my_stod(stringData[ofst +i*col_num]); //
    double x1 = token.CoherentScatteringCrossSection_cm2_per_gram = my_stod(stringData[ofst +1 +i*col_num]); //
    double x2 = token.IncoherentScatteringCrossSection_cm2_per_gram = my_stod(stringData[ofst +2 +i*col_num]); //
    double x3 = token.PhotoElectricCrossSection_cm2_per_gram = my_stod(stringData[ofst +3 +i*col_num]); //
    double sx = token.MassAbsorptionCoefficient_cm2_per_gram = x1 +x2 +x3;

    out.push_back(token);
  }
}


double ConvertAngsromsToMeV(const double energy_angstroms)
{
  double energyKeV;
  if(energy_angstroms > 0.0){
    energyKeV = 12.40/energy_angstroms; // Based on formula: E = hc/lambda
  }else{
    energyKeV=0.0;
  }
  double energyMeV = energyKeV/1000.0;
  return energyMeV;
};





void Element::GenerateK_alpha(const std::string &filepath,
			      const int Z)
{

  /////////////////////
  // Open file
  /////////////////////
  std::ifstream ifs(filepath.c_str());
  if(ifs.fail()){
    std::cout << "file not found:" << filepath << std::endl;
  }
  
  
  /////////////////////
  // Read all data
  /////////////////////
  std::string str;
  std::vector<double> data;
  
  while( getline(ifs, str) ){
    data.push_back( my_stod(str) );
  }


  /////////////////////
  // Generate Kalpha
  /////////////////////
  int ofst=1;
  double K_alpha_angstroms = data[(Z-1)*2 +1];
  this->K_alpha_MeV = ConvertAngsromsToMeV( K_alpha_angstroms );
}



SharedElementPtr Element::CreateElement(const MaterialDataFiles_t &dataFiles,
					const int Z){
  
  SharedElementPtr ret(new Element);
  ret->Z =Z;
  ret->GenerateAtomicFormFactor( dataFiles.IncoherentFilepath, Z, ret->IncoherentScatteringFormFactor);
  ret->GenerateAtomicFormFactor( dataFiles.CoherentFilepath, Z, ret->CoherentScatteringFormFactor);
  ret->GeneratePhotonCrossSection( dataFiles.PhotonCrossSectionDir, Z, ret->PhotonCrossSection);
  ret->GenerateK_alpha( dataFiles.K_alphaFilepath, Z);

  return ret;
}





class PredicateGreaterEnergy
{
public:
  PredicateGreaterEnergy(double val_in) : Val(val_in){};
  double Val;
  inline bool operator()(PhotonCrossSection_t &in){ return this->Val < in.Energy_MeV;}
};



PhotonCrossSection_t Element::GetPhotonCrossSectionByEnrgy(const double Energy_MeV)
{
  
  // Find energy range
  PredicateGreaterEnergy pred(Energy_MeV);

  std::vector<PhotonCrossSection_t>::iterator it_bigger, it_smaller;


  // Linear search(temporary)
  it_bigger = std::find_if(this->PhotonCrossSection.begin(),
			   this->PhotonCrossSection.end(),
			   pred );
  
  it_smaller = it_bigger-1;


  // Calc PhotonCrossSection corresponding to energy
  PhotonCrossSection_t ret;

  ret.Energy_MeV = Energy_MeV;


  double s1 = LogComplement(it_smaller->Energy_MeV,
			    it_bigger->Energy_MeV,
			    it_smaller->PhotoElectricCrossSection_cm2_per_gram,
			    it_bigger->PhotoElectricCrossSection_cm2_per_gram,
			    Energy_MeV);
  ret.PhotoElectricCrossSection_cm2_per_gram =s1;
  

  double s2 = LogComplement(it_smaller->Energy_MeV,
			    it_bigger->Energy_MeV,
			    it_smaller->CoherentScatteringCrossSection_cm2_per_gram,
			    it_bigger->CoherentScatteringCrossSection_cm2_per_gram,
			    Energy_MeV);
  ret.CoherentScatteringCrossSection_cm2_per_gram = s2;

    
  double s3 = LogComplement(it_smaller->Energy_MeV,
			    it_bigger->Energy_MeV,
			    it_smaller->IncoherentScatteringCrossSection_cm2_per_gram,
			    it_bigger->IncoherentScatteringCrossSection_cm2_per_gram,
			    Energy_MeV);
  ret.IncoherentScatteringCrossSection_cm2_per_gram = s3;
  

  ret.MassAbsorptionCoefficient_cm2_per_gram = s1 +s2 +s3;
  return ret;
};





void Element::PrintElementInfo()
{
  std::cout << "ElementName: " << this->ElementName << std::endl;
  printf("AtomicWeight: %e\n", this->AtomicWeight);
  printf("K_alpha(MeV): %e\n", this->K_alpha_MeV );  
}




///////////////////////
//
// Compound
//
///////////////////////
struct ChemicalCompositionNumber_t{
  int Z;
  int Quantity;
};



////////////////////////
// Compound json reader
////////////////////////
void ParseChemicalCompoundJson(const std::string &jsonpath,
			       const std::string &compoundNameToGenerate,
			       std::string &compoundName,
			       double &density,
			       std::vector<ChemicalCompositionNumber_t> &chemicalCompositionNumber)
{
  //Open file
  std::ifstream inputStream;
  std::string thisLine;
  inputStream.open(jsonpath.c_str());
  if (!inputStream.is_open()){
    std::cout << "cannot open file: " << jsonpath << std::endl;
    exit(1);
  }

  std::stringstream sstream;
  while (getline(inputStream, thisLine)){
    sstream << thisLine;
  }
  inputStream.close();
  

  //Parse json
  picojson::value v; 
  picojson::parse(v, sstream);
  picojson::object& all = v.get<picojson::object>();

  //Get compound object
  picojson::object& compound = all[compoundNameToGenerate.c_str()].get<picojson::object>();
  
  //Substitute member
  density = compound["Density"].get<double>();
  compoundName = compound["Name"].get<std::string>();
  

  //Generate chemicalCompositionNumber
  picojson::array& elem = compound["Element"].get<picojson::array>();

  for (picojson::array::iterator it = elem.begin(); it != elem.end(); it++){
    
    ChemicalCompositionNumber_t ccn;
    picojson::object& tmpObject = it->get<picojson::object>();

    ccn.Z = (int)tmpObject["AtomicNumber"].get<double>();
    ccn.Quantity = (int)tmpObject["Quantity"].get<double>();
    
    chemicalCompositionNumber.push_back(ccn);
  }
}



//////////////////////////////
// Compose atmic form factor
//////////////////////////////
void Compound::GenerateComposedCoherentScatteringFormFactor(std::vector<ChemicalComposition_t> &ChemicalComposition)
{

  //////////////////////
  // Compose
  //////////////////////
  for(int i=0; i< ChemicalComposition[0].Element->CoherentScatteringFormFactorSize(); i++){

    AtomicFormFactor_t tmpAff;
    tmpAff.MomentumTransfer_one_per_angstrom = ChemicalComposition[0].Element->GetCoherentMomentumTransferByIndex(i);
    double sum =0.0;

    for(int j=0; j < ChemicalComposition.size(); j++){
      int n = ChemicalComposition[j].Quantity;
      double F = ChemicalComposition[j].Element->GetCoherentFormFactorByIndex(i);
      sum += n *F*F;
    }

    tmpAff.F =sqrt(sum);
    this->ComposedCoherentScatteringFormFactor.push_back(tmpAff);
  }
  
}


//////////////////////
// Generate cumulutive distribution
/////////////////////
inline double CoherentFormFactorDistribution(const double MomentumTransfer,
				      const double FormFactor)
{
  return FormFactor *FormFactor *MomentumTransfer;
}


void Compound::CalcComposedCoherentScatteringFormFactorDistribution()
{  
 
  double cumulativeDistribution=0.0;
  // idx =0;
  {
    AtomicFormFactor_t tmpAff;
    tmpAff.MomentumTransfer_one_per_angstrom =this->ComposedCoherentScatteringFormFactor[0].MomentumTransfer_one_per_angstrom;
    tmpAff.F =cumulativeDistribution;
    this->ComposedCoherentScatteringFormFactorDistribution.push_back(tmpAff);
  }

  // idx >1
  for(int i=0; i< this->ComposedCoherentScatteringFormFactor.size()-1; i++){

    double x0 =this->ComposedCoherentScatteringFormFactor[i].MomentumTransfer_one_per_angstrom;
    double F0 =this->ComposedCoherentScatteringFormFactor[i].F;
    double FD0 =CoherentFormFactorDistribution(x0, F0);
    
    double x1 =this->ComposedCoherentScatteringFormFactor[i+1].MomentumTransfer_one_per_angstrom;
    double F1 =this->ComposedCoherentScatteringFormFactor[i+1].F;
    double FD1 =CoherentFormFactorDistribution(x1, F1);

    cumulativeDistribution +=TrapezoidalArea(x0, x1, FD0, FD1);
    
    AtomicFormFactor_t tmpAff;
    tmpAff.MomentumTransfer_one_per_angstrom =x1;
    tmpAff.F =cumulativeDistribution;
    this->ComposedCoherentScatteringFormFactorDistribution.push_back(tmpAff);
  }

  ///////////////////////
  // Set distribution's max
  ///////////////////////
  int maxIdx=ComposedCoherentScatteringFormFactorDistribution.size()-1;
  double cfmax =this->ComposedCoherentScatteringFormFactorDistribution[maxIdx].F;
  ComposedCoherentScatteringFormFactorDistributionMax =cfmax;
}



////////////////////
// Incoherent(Compton)
////////////////////
void Compound::GenerateComposedIncoherentScatteringFormFactor(std::vector<ChemicalComposition_t> &ChemicalComposition)
{

  for(int i=0; i< ChemicalComposition[0].Element->IncoherentScatteringFormFactorSize(); i++){

    AtomicFormFactor_t tmpAff;
    tmpAff.MomentumTransfer_one_per_angstrom = ChemicalComposition[0].Element->GetIncoherentMomentumTransferByIndex(i);
    double sum =0.0;

    for(int j=0; j < ChemicalComposition.size(); j++){
      int n = ChemicalComposition[j].Quantity;
      sum += n * ChemicalComposition[j].Element->GetIncoherentFormFactorByIndex(i);
    }

    tmpAff.F =sum;
    this->ComposedIncoherentScatteringFormFactor.push_back(tmpAff);
  }
  
  //////////////////////////
  //Calc max form factor
  //////////////////////////
  int maxIdx =this->ComposedIncoherentScatteringFormFactor.size()-1;
  this->ComposedIncoherentScatteringFormFactorMax =this->ComposedIncoherentScatteringFormFactor[maxIdx].F;
}



/////////////////////////
// Compound Factory
/////////////////////////
SharedCompoundPtr Compound::CreateCompound(const std::string &jsonpath,
					   const std::string &compoundNameToGenerate,
					   const MaterialDataFiles_t & dataFiles)
{
  
  SharedCompoundPtr ret(new Compound);
    
  std::vector<ChemicalCompositionNumber_t> chemicalCompositionNumber;
  ParseChemicalCompoundJson(jsonpath,
			    compoundNameToGenerate,
			    ret->CompoundName,
			    ret->Density_gram_per_cm3,
			    chemicalCompositionNumber);

  //Convert chemicalCompositionNumber to vector<ChemicalComposition_t>
  for(std::vector<ChemicalCompositionNumber_t>::iterator it =chemicalCompositionNumber.begin();
      it != chemicalCompositionNumber.end();
      it++){


    ChemicalComposition_t tmp_comp;

    tmp_comp.Quantity = it->Quantity;
    tmp_comp.Element = Element::CreateElement(dataFiles,
					      it->Z);

    ret->ChemicalComposition.push_back(tmp_comp);
  }

  //Incoherent
  ret->GenerateComposedIncoherentScatteringFormFactor( ret->ChemicalComposition );

  //Coherent
  ret->GenerateComposedCoherentScatteringFormFactor( ret->ChemicalComposition );
  ret->CalcComposedCoherentScatteringFormFactorDistribution();
  return ret;
}




///////////////////////////////////////
// Get photon cross section by energy
///////////////////////////////////////
PhotonCrossSection_t Compound::GetComposedPhotonCrossSectionByEnergy(const double Energy_MeV)
{

  double photoEle =0.0;
  double coh =0.0;
  double incoh =0.0;
  double absorp =0.0;
  double denomi =0.0;

  for(int i=0; i< this->ChemicalComposition.size(); i++){
    int Q =ChemicalComposition[i].Quantity;
    double aw =ChemicalComposition[i].Element->GetAtomicWeight();
    double coef =Q*aw;
    PhotonCrossSection_t pc =ChemicalComposition[i].Element->GetPhotonCrossSectionByEnrgy(Energy_MeV);
    
    photoEle += coef* pc.PhotoElectricCrossSection_cm2_per_gram;
    coh += coef* pc.CoherentScatteringCrossSection_cm2_per_gram;
    incoh += coef* pc.IncoherentScatteringCrossSection_cm2_per_gram;
    absorp += coef* pc.MassAbsorptionCoefficient_cm2_per_gram;
    denomi += coef;
  }
 
  PhotonCrossSection_t total;
  total.Energy_MeV =Energy_MeV;
  total.PhotoElectricCrossSection_cm2_per_gram =photoEle /denomi;
  total.CoherentScatteringCrossSection_cm2_per_gram =coh /denomi;
  total.IncoherentScatteringCrossSection_cm2_per_gram =incoh /denomi;
  total.MassAbsorptionCoefficient_cm2_per_gram = absorp /denomi;

  return total;
};
  




///////////////////////////////////////////////
// Get atomic form factor by momentum transfer
///////////////////////////////////////////////
class PredicateGreaterMomentumTransfer
{
public:
  PredicateGreaterMomentumTransfer(double val_in) : Val(val_in){};
  double Val;
  inline bool operator()(AtomicFormFactor_t &in){ return this->Val < in.MomentumTransfer_one_per_angstrom; }
};



double Compound::GetComposedCoherentScatteringFormFactorByMomentumTransfer(const double x_one_per_angstrom)
{
  // Find energy range
  PredicateGreaterMomentumTransfer pred(x_one_per_angstrom);
  std::vector<AtomicFormFactor_t>::iterator it_bigger, it_smaller;
  
  
  // Linear search(temporary)
  it_bigger = std::find_if(this->ComposedCoherentScatteringFormFactor.begin(),
			   this->ComposedCoherentScatteringFormFactor.end(),
			   pred );
  
  it_smaller = it_bigger-1;
  
  return LinearComplement(it_smaller->MomentumTransfer_one_per_angstrom,
			  it_bigger->MomentumTransfer_one_per_angstrom,
			  it_smaller->F,
			  it_bigger->F,
			  x_one_per_angstrom);
}


double Compound::GetComposedCoherentScatteringFormFactorDistributionByMomentumTransfer(const double x_one_per_angstrom)
{
  // Find energy range
  PredicateGreaterMomentumTransfer pred(x_one_per_angstrom);
  std::vector<AtomicFormFactor_t>::iterator it_bigger, it_smaller;
  
  
  // Linear search(temporary)
  it_bigger = std::find_if(this->ComposedCoherentScatteringFormFactorDistribution.begin(),
			   this->ComposedCoherentScatteringFormFactorDistribution.end(),
			   pred );
  
  it_smaller = it_bigger-1;
  
  return LinearComplement(it_smaller->MomentumTransfer_one_per_angstrom,
			  it_bigger->MomentumTransfer_one_per_angstrom,
			  it_smaller->F,
			  it_bigger->F,
			  x_one_per_angstrom);
}



class PredicateGreaterFormFactor
{
public:
  PredicateGreaterFormFactor(double val_in) : Val(val_in){};
  double Val;
  inline bool operator()(AtomicFormFactor_t &in){ return this->Val < in.F;}
};



double Compound::GetMomentumTransferByCoposedCoherentScatteringFormFactorDistribution(const double F)
{
  // Find energy range
  PredicateGreaterFormFactor pred(F);
  std::vector<AtomicFormFactor_t>::iterator it_bigger, it_smaller;
  
  
  // Linear search(temporary)
  it_bigger = std::find_if(this->ComposedCoherentScatteringFormFactorDistribution.begin(),
			   this->ComposedCoherentScatteringFormFactorDistribution.end(),
			   pred );
  
  it_smaller = it_bigger-1;
  
  return LinearComplement(it_smaller->F,
			  it_bigger->F,
			  it_smaller->MomentumTransfer_one_per_angstrom,
			  it_bigger->MomentumTransfer_one_per_angstrom,
			  F);
}



/////////////////
// Incoherent
/////////////////
double Compound::GetComposedIncoherentScatteringFormFactorByMomentumTransfer(const double x_one_per_angstrom)
{
  // Find energy range
  PredicateGreaterMomentumTransfer pred(x_one_per_angstrom);
  std::vector<AtomicFormFactor_t>::iterator it_bigger, it_smaller;
  
  
  // Linear search(temporary)
  it_bigger = std::find_if(this->ComposedIncoherentScatteringFormFactor.begin(),
			   this->ComposedIncoherentScatteringFormFactor.end(),
			   pred );
  
  it_smaller = it_bigger-1;

  return LinearComplement(it_smaller->MomentumTransfer_one_per_angstrom,
			  it_bigger->MomentumTransfer_one_per_angstrom,
			  it_smaller->F,
			  it_bigger->F,
			  x_one_per_angstrom);
}


//////////////////////////////
// Get composed K_alpha
//////////////////////////////
ComposedK_alpha Compound::GetComposedK_alphaByEnergy(const double energy_MeV)
{

  ComposedK_alpha ret;

  for(int i=0; i< this->ChemicalComposition.size(); i++){

    int Q =this->ChemicalComposition[i].Quantity;
    double aw =this->ChemicalComposition[i].Element->GetAtomicWeight();
    double coef =Q*aw;
    PhotonCrossSection_t pc =this->ChemicalComposition[i].Element->GetPhotonCrossSectionByEnrgy(energy_MeV);    
    
    K_alphaInfo_t token;
    token.RelativeProbability =coef* pc.PhotoElectricCrossSection_cm2_per_gram;
    token.K_alpha_MeV =this->ChemicalComposition[i].Element->GetK_alpha_MeV();
    token.Z =this->ChemicalComposition[i].Element->GetAtomicNumber();
    ret.push_back(token);
  }
  
  return ret;
}



//////////////////
// Print info
/////////////////
void Compound::PrintCompoundInfo(){

  printf("Compound Name: %s\n", this->CompoundName.c_str());
  printf("Density: %e\n", this->Density_gram_per_cm3);
  printf("\nElements Info\n");

  for(int i=0; i< this->ChemicalComposition.size(); i++ ){
    printf("%d:\n", i+1);
    this->ChemicalComposition[i].Element->PrintElementInfo();
    std::cout << "Quantity: " << this->ChemicalComposition[i].Quantity << std::endl;
    printf("\n");
  }  
}


void PrintAtomicFormFacter(std::vector<AtomicFormFactor_t> &in)
{
  for(int i=0; i< in.size(); i++){
    AtomicFormFactor_t Aff = in[i];    
    printf("x: %e ", Aff.MomentumTransfer_one_per_angstrom);
    printf("F: %e\n", Aff.F);
  }
}

void Compound::PrintComposedCoherentScatteringFormFactor()
{
  std::cout << "CoherentFormFactor" << std::endl;
  PrintAtomicFormFacter(this->ComposedCoherentScatteringFormFactor);
};



void Compound::PrintComposedCoherentScatteringFormFactorDistribution()
{
  std::cout << "CoherentFormFactorDistribution" << std::endl;
  PrintAtomicFormFacter(this->ComposedCoherentScatteringFormFactorDistribution);
};


void Compound::PrintComposedIncoherentScatteringFormFactor()
{
  std::cout << "IncoherentFormFactor" << std::endl;
  PrintAtomicFormFacter(this->ComposedIncoherentScatteringFormFactor);
};
