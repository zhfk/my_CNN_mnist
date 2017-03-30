
# This is a GNU Makefile.

# It can be used to compile an OpenCL program with
# the Altera Beta OpenCL Development Kit.
# See README.txt for more information.


# You must configure ALTERAOCLSDKROOT to point the root directory of the Altera SDK for OpenCL
# software installation.
# See doc/getting_started.txt for more information on installing and
# configuring the Altera SDK for OpenCL.


# Creating a static library
TARGET = my_CNN_mnist

# Where is the Altera SDK for OpenCL software?
ifeq ($(wildcard $(ALTERAOCLSDKROOT)),)
$(error Set ALTERAOCLSDKROOT to the root directory of the Altera SDK for OpenCL software installation)
endif
ifeq ($(wildcard $(ALTERAOCLSDKROOT)/host/include/CL/opencl.h),)
$(error Set ALTERAOCLSDKROOT to the root directory of the Altera SDK for OpenCL software installation.)
endif

# Libraries to use, objects to compile
SRCS = cnn_main.cpp \
		Layer.cpp	\
		util.cpp	\
		CNN.cpp		\
		tool.cpp
SRCS_FILES = $(foreach F, $(SRCS), host/src/$(F))

# arm cross compiler
CROSS-COMPILE = arm-linux-gnueabihf-

# OpenCL compile and link flags.
AOCL_COMPILE_CONFIG=$(shell aocl compile-config --arm)  -Ihost/inc
AOCL_LINK_CONFIG=$(shell aocl link-config --arm) -lrt -std=gnu++11


# Make it all!
all : 
	$(CROSS-COMPILE)g++ $(SRCS_FILES)  -o $(TARGET)  $(AOCL_COMPILE_CONFIG) $(AOCL_LINK_CONFIG)


# Standard make targets
clean :
	@rm -f *.o $(TARGET)
