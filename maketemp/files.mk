DETECTOR_DIR =$(TOP)/Src/Detector
SHAPE_DIR =$(TOP)/Src/Shape
PHOTON_DIR =$(TOP)/Src/Photon
PHOTON_GEN_DIR =$(TOP)/Src/PhotonGenerator
RANDOM_DIR =$(TOP)/Src/RandomNumber
QUATANION_DIR =$(TOP)/Src/Quaternion
MATH_TOOLS_DIR =$(TOP)/Src/MathematicalTools/include
COMPOUND_DIR =$(TOP)/Src/Material
PHANTOM_TREE_DIR =$(TOP)/Src/PhantomTree
JSON_LIB_DIR =$(TOP)/Src/Libs/picojson-master
FLYING_PHOTON_DIR =$(TOP)/Src/FlyingPhoton
UTILS_DIR =$(TOP)/Src/Util

INC_DIR += -I./
INC_DIR += -I$(DETECTOR_DIR)
INC_DIR += -I$(SHAPE_DIR)
INC_DIR += -I$(PHOTON_DIR)
INC_DIR += -I$(PHOTON_GEN_DIR)
INC_DIR += -I$(FLYING_PHOTON_DIR)
INC_DIR += -I$(RANDOM_DIR)
INC_DIR += -I$(QUATANION_DIR)
INC_DIR += -I$(MATH_TOOLS_DIR)
INC_DIR += -I$(COMPOUND_DIR)
INC_DIR += -I$(PHANTOM_TREE_DIR)
INC_DIR += -I$(UTILS_DIR)
INC_DIR += -I$(JSON_LIB_DIR)

vpath %.cpp ./
vpath %.cpp $(DETECTOR_DIR)
vpath %.cpp $(SHAPE_DIR)
vpath %.cpp $(PHOTON_DIR)
vpath %.cpp $(PHOTON_GEN_DIR)
vpath %.cpp $(RANDOM_DIR)
vpath %.cpp $(QUATANION_DIR)
vpath %.cpp $(MATH_TOOLS_DIR)
vpath %.cpp $(COMPOUND_DIR)
vpath %.cpp $(PHANTOM_TREE_DIR)
vpath %.cpp $(FLYING_PHOTON_DIR)
vpath %.cpp $(UTILS_DIR)
vpath %.cpp $(JSON_LIB_DIR)

CPPSRC += Detector.cpp
CPPSRC += FlyingPhoton.cpp
CPPSRC += Shape.cpp
CPPSRC += Compound.cpp
CPPSRC += Photon.cpp
CPPSRC += PhotonGenerator.cpp
CPPSRC += PhantomTree.cpp

