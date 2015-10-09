#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <tr1/memory>
#include <algorithm>
#include "Compound.h"
#include "Shape.h"
#include "PhantomTree.h"
#include "picojson.h"
#include "FlyingPhoton.h"

const double EPSILON_05=0.000001;


/////////////////////////
//
// Phantom Node
//
/////////////////////////
SharedPhantomNodePtr PhantomNode::CreatePhantomNode()
{
  SharedPhantomNodePtr obj( new PhantomNode() );
  obj->Self = obj;
  return obj;
};



PhantomNode::PhantomNode()
{};




PhantomNode::~PhantomNode()
{};


void PhantomNode::SetParent(const SharedPhantomNodePtr parent_in)
{
  this->Parent =parent_in;
};



void PhantomNode::AddChild(const SharedPhantomNodePtr child_in)
{
  this->Children.push_back( child_in );
};


WeakPhantomNodePtr PhantomNode::GetSelf()
{
  return Self;
};


std::vector<WeakPhantomNodePtr> PhantomNode::GetChildren()
{
  return Children;
};


WeakPhantomNodePtr PhantomNode::GetParent()
{
  return Parent;
};



void PhantomNode::SetMyName(const std::string &myname)
{
  this->MyName = myname;
};



void PhantomNode::SetParentName(const std::string &pname)
{
  this->ParentName = pname;
};



std::string PhantomNode::GetMyName()
{
  return this->MyName;
};



std::string PhantomNode::GetParentName()
{
  return this->ParentName;
};




void PhantomNode::PrintSubTree(int &lineSize)
{
  
  for(int i=0; i< lineSize; i++){
    std::cout << " ";
  }  
  std::cout << "->" << this->MyName << std::endl;

  int strSize =this->MyName.length();
  lineSize += strSize;

  if( !this->Children.empty() ){
    for(int i=0; i<this->Children.size(); i++){      
      this->Children[i].lock()->PrintSubTree(lineSize);
    }
  }
  
  lineSize -= strSize;
};



void PhantomNode::PrintPhantomNodeCompoundInfo()
{    
  std::cout << std::endl << "@@ " << this->MyName << " @@" << std::endl << std::endl;
  this->Compound->PrintCompoundInfo();

  if( !this->Children.empty() ){
    for(int i=0; i<this->Children.size(); i++){  
      this->Children[i].lock()->PrintPhantomNodeCompoundInfo();
    }
  }
};




void PhantomNode::SetShape(const SharedShapePtr shape_in)
{
  this->Shape =shape_in;
};



void PhantomNode::SetCompound(const SharedCompoundPtr compound_in)
{
  this->Compound =compound_in;
};




SharedShapePtr PhantomNode::GetShape()
{
  return this->Shape;
};



SharedCompoundPtr PhantomNode::GetCompound()
{
  return this->Compound;
};


/////////////
// geometric
////////////
bool PhantomNode::GetIntersectionPoints(const Ray& ray,
					PointVector_t &p1,
					PointVector_t &p2)
{
  return this->Shape->GetIntersectionPoints(ray, p1, p2);
};



bool PhantomNode::IsInside(const PointVector_t& p)
{
  return this->Shape->IsInside(p);
};
  


bool PhantomNode::GetForwardNodeRayParameter(const Ray& ray, double& t)
{
  double t1, t2;
  if( this->Shape->GetIntersectionPointsRayParameter(ray, t1, t2) ){
    
    if(t1 < EPSILON_05 && t2 < EPSILON_05){ return false; }
    
    t1 =std::max(EPSILON_05, t1);
    t2 =std::max(EPSILON_05, t2);
    t =std::min(t1, t2);
    return true;
  }
  return false;
}



//////////////////////////
//
// Phantom Tree
//
/////////////////////////
void GetBallParameter(picojson::object& paramObject,
		      ShapeParameter_t &shapeParam)
{
  
  double rad = paramObject["Rad"].get<double>();
  picojson::object& center = paramObject["Center"].get<picojson::object>();
  PointVector_t p;
  p.x = center["x"].get<double>();
  p.y = center["y"].get<double>();
  p.z = center["z"].get<double>();
  
  shapeParam.Radius = rad;
  shapeParam.Center = p;
}



SharedShapePtr ParseShapeParameter(picojson::object& shape)
{

  ShapeParameter_t shapeParam;

  // get type
  shapeParam.Type = shape["Type"].get<std::string>();

  // get parameter
  picojson::object& paramObject = shape["Parameter"].get<picojson::object>();
  
  if(shapeParam.Type == "Ball"){
    GetBallParameter(paramObject, shapeParam);
  }

  //create
  SharedShapePtr ret= Shape::Create(shapeParam);
  return ret;
   
};



void ReadJsonFileToStringStream(const std::string &filePath,
				std::stringstream &sstream)
{

  //open file
  std::ifstream inputStream;
  std::string thisLine;
  inputStream.open(filePath.c_str());
  if (!inputStream.is_open()){
    std::cerr << "cannot open file:" << filePath <<std::endl;
    exit(1);
  }

  //read file
  while (getline(inputStream, thisLine)){
    sstream << thisLine;
  }
  inputStream.close();

};




void PhantomTree::ParsePhantomJson(const std::string &phantomJsonPath,
				   const std::string &compoundJsonPath,
				   const MaterialDataFiles_t &dataFiles)
{

  //read file
  std::stringstream sstream;
  ReadJsonFileToStringStream(phantomJsonPath, sstream);


  // parse json
  picojson::value v; 
  picojson::parse(v, sstream);

  picojson::object& all = v.get<picojson::object>();
  picojson::array& phantom = all["Phantom"].get<picojson::array>();

  
  // create phantom nodes  
  for (picojson::array::iterator it = phantom.begin(); it != phantom.end(); it++){

    SharedPhantomNodePtr pn = PhantomNode::CreatePhantomNode();
    picojson::object &tmpObject = it->get<picojson::object>();
      
    /// Name
    std::string myname = tmpObject["Name"].get<std::string>();
    pn->SetMyName(myname);
    std::string pname = tmpObject["Parent"].get<std::string>();
    pn->SetParentName(pname);
         
    /// Shape
    picojson::object& shape = tmpObject["Shape"].get<picojson::object>();
    pn->SetShape( ParseShapeParameter(shape) );

    // Compound
    std::string compoundName = tmpObject["Compound"].get<std::string>();    
    pn->SetCompound( Compound::CreateCompound(compoundJsonPath, compoundName, dataFiles) );
    
    // add node
    this->Nodes.push_back( pn );
  }  
};




void PhantomTree::ConstructTree()
{
  // Find Root
  for(std::vector<SharedPhantomNodePtr>::iterator it1 = this->Nodes.begin();
      it1 != this->Nodes.end(); it1++){
    if(it1->get()->GetParentName() == "Root" ){
      this->SetRootNode( *it1 );
    }
  }

  for(std::vector<SharedPhantomNodePtr>::iterator it1 = this->Nodes.begin();
      it1 != this->Nodes.end(); it1++){

    for(std::vector<SharedPhantomNodePtr>::iterator it2 = this->Nodes.begin();
	it2 != this->Nodes.end(); it2++){

      if(it1->get()->GetMyName() == it2->get()->GetParentName() ){
	it1->get()->AddChild( *it2 );
	it2->get()->SetParent( *it1 );
      }
      
    }
  }
};


SharedPhantomTreePtr PhantomTree::Create(const std::string &phantomJsonPath,
					 const std::string &compoundJsonPath,
					 const MaterialDataFiles_t &dataFiles)
{
  SharedPhantomTreePtr ret(new PhantomTree);
  
  ret->ParsePhantomJson(phantomJsonPath,
			compoundJsonPath,
			dataFiles);
  
  ret->ConstructTree();
  
  return ret;
}



/////////////////
// Photon Transport Control
/////////////////
bool PhantomTree::SetCurrentChildrenNodeToPhoton(const WeakPhantomNodePtr node,
						 SharedFlyingPhotonPtr &photon)
{
  std::vector<WeakPhantomNodePtr> children =node.lock()->GetChildren();
  
  for(int i=0; i<children.size(); i++){
    if ( children[i].lock()->IsInside( photon.get()->GetCurrentPosition() ) ){
      photon.get()->SetCurrentPhantomNode( children[i].lock() );
      SetCurrentChildrenNodeToPhoton( children[i], photon );
      return true;
    }
  }
  return false;
  
}



bool PhantomTree::SetFirstPhantomNodeToPhoton(SharedFlyingPhotonPtr &photon)
{

  SharedPhantomNodePtr root =GetRootNode();

  if( root.get()->IsInside( photon->GetCurrentPosition() ) ){ //Start Inside of Tree
  
    return SetCurrentChildrenNodeToPhoton(root, photon);

  }else{ //Start Outside of Tree
    
    double t0;    
    if( !root.get()->GetForwardNodeRayParameter( photon->GetPhotonRay(1.0), t0) ){
      return false;
    }//not captured

    photon->MoveForwardByRayParameter(t0);
    photon->SetCurrentPhantomNode(root);
    return true;

  }
  return false;
}



void PhantomTree::TransportPhotonThroughPhantomTree(UniformRandomGenerator0to1 &random,
						    SharedFlyingPhotonPtr &photon)
{

  //Set First Phantom Node
  if( !this->SetFirstPhantomNodeToPhoton(photon) ){
    return;
  }

  //First Move
  photon->Update(random);

  //Update Photon
  SharedPhantomNodePtr root=this->GetRootNode();
  int count=0;
  while( root->IsInside( photon->GetCurrentPosition() ) && !photon->IsAbsorbed() && !photon->DoesFlyAway() ){
    count++;
    if(count > 10000){
      //std::cout << "update invalid error" << std::endl;
      return;
    }
    photon->Update(random);
  }
  return;
}








////////////////
// print
////////////////
void PhantomTree::PrintPhantomTreeStructure()
{
  int lineSize=0;
  for(std::vector<SharedPhantomNodePtr>::iterator it= this->Nodes.begin();
      it != this->Nodes.end(); it++){
    
    if(it->get()->GetParentName() == "Root"){
      it->get()->PrintSubTree(lineSize);
    }
  }
};




void PhantomTree::PrintPhantomTreeCompoundInfo()
{

  for(std::vector<SharedPhantomNodePtr>::iterator it= this->Nodes.begin();
      it != this->Nodes.end(); it++){
    
    if(it->get()->GetParentName() == "Root"){
      it->get()->PrintPhantomNodeCompoundInfo();
    }
  }
};
