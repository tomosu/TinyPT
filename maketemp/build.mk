#######################################################################
#                                                                     #
#                        Makefile ( GNU make )                        #
#                                                                     #
#######################################################################
TOP = ../..

CPPSRC += test_template.cpp
#CPPSRC += test_modules.cpp
CPPSRC += PSF_convolution.cpp
CPPSRC += ideal_projection_generate.cpp
CPPSRC += montecarlo_projection_generate.cpp
CPPSRC += rayleigh_scatter_noise_generate.cpp
CPPSRC += ct_property.cpp
CPPSRC += rayleigh_scatter.cpp
CPPSRC += compton_scatter.cpp
CPPSRC += calc_atomic_form_factor.cpp
CPPSRC += determine_interaction.cpp
CPPSRC += free_path.cpp
CPPSRC += get_node.cpp
CPPSRC += mathematical_tools.cpp
CPPSRC += libarg.cpp
CPPSRC += libini.cpp
CPPSRC += create_material_data.cpp
CPPSRC += photon.cpp
CPPSRC += file_util.cpp
CPPSRC += setup_irradiated_body.cpp
CPPSRC += vibration.cpp
CPPSRC += xray_table_gen.cpp


CSRC += dSFMT.c
CSRC += mt19937ar.c

CCOPT  = -O0 -g
TARGET = tst

LIBS = -lgomp

INC_DIR += -I./include
INC_DIR += -I../../lib
INC_DIR += -I../common/include
INC_DIR += -I../dsfmt/dSFMT-src-2.2
INC_DIR += -I../mt

vpath %.cpp ../common
vpath %.cpp ../../lib
vpath %.c ../mt
vpath %.c ../dsfmt/dSFMT-src-2.2

include $(TOP)/maketemp/default_host.mk


