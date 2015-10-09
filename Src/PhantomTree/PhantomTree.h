#pragma once
#include "Compound.h"
#include "Shape.h"
#include "FlyingPhoton.h"
#include "RandomNumberGenerator.h"
#include <tr1/memory>
#include <vector>


class PhantomNode;
typedef std::tr1::shared_ptr<PhantomNode> SharedPhantomNodePtr;
typedef std::tr1::weak_ptr<PhantomNode> WeakPhantomNodePtr;

class PhantomTree;
typedef std::tr1::shared_ptr<PhantomTree> SharedPhantomTreePtr;

class FlyingPhoton;
typedef std::tr1::shared_ptr<FlyingPhoton> SharedFlyingPhotonPtr;



////////////////////
//
// Phantom Node
//
////////////////////
class PhantomNode{

  friend class PhantomTree;

 public:
  static SharedPhantomNodePtr CreatePhantomNode();

  PhantomNode();
  ~PhantomNode();

  // geometric interaction
  bool GetIntersectionPoints(const Ray&, PointVector_t&, PointVector_t&);
  bool IsInside(const PointVector_t&);
  bool GetForwardNodeRayParameter(const Ray&, double&);
  
  // get parent & children
  std::vector<WeakPhantomNodePtr> GetChildren();
  WeakPhantomNodePtr GetParent();

  // get property
  SharedCompoundPtr GetCompound();
  SharedShapePtr GetShape();
  std::string GetMyName();
  std::string GetParentName();


  // print
  void PrintSubTree(int &lineSize);
  void PrintPhantomNodeCompoundInfo();
    
  
private:
  // tree build
  void SetParent(const SharedPhantomNodePtr);
  void AddChild(const SharedPhantomNodePtr);
  WeakPhantomNodePtr GetSelf();

  // setter getter
  void SetShape(const SharedShapePtr);
  void SetCompound(const SharedCompoundPtr);
  
  void SetMyName(const std::string &);
  void SetParentName(const std::string &);
  
  // data
  std::string MyName;
  std::string ParentName;
  WeakPhantomNodePtr Self;
  WeakPhantomNodePtr Parent;
  std::vector<WeakPhantomNodePtr> Children;

  // shape & compound
  SharedShapePtr Shape;
  SharedCompoundPtr Compound;
};



////////////////////
//
// Phantom Tree
//
////////////////////
class PhantomTree{

public:
  static SharedPhantomTreePtr Create(const std::string &phantomJsonPath,
				     const std::string &compoundJsonPath,
				     const MaterialDataFiles_t &dataFiles);

  void PrintPhantomTreeStructure();

  void PrintPhantomTreeCompoundInfo();

  void TransportPhotonThroughPhantomTree(UniformRandomGenerator0to1 &random,
					 SharedFlyingPhotonPtr &);

  
private:
  
  void ParsePhantomJson(const std::string &phantomJsonPath,
			const std::string &compoundJsonPath,
			const MaterialDataFiles_t &dataFiles);
  
  void ConstructTree();

  SharedPhantomNodePtr GetRootNode() const { return RootNode; }

  void SetRootNode(const SharedPhantomNodePtr &in)
  {
    this->RootNode =in;
  };

  bool SetCurrentChildrenNodeToPhoton(const WeakPhantomNodePtr node,
				      SharedFlyingPhotonPtr &photon);

  bool SetFirstPhantomNodeToPhoton(SharedFlyingPhotonPtr &photon);


  //data
  SharedPhantomNodePtr RootNode;
  std::vector<SharedPhantomNodePtr> Nodes;  
};
