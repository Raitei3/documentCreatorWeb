##############################
# EXAMPLE 2                  #
# created by T.DESCOMBES     #
#                    2015    #
##############################

OUTPUTFILENAME = PrecompiledRepository.cc
NAVAJO_PRECOMPILER_EXEC = ../bin/navajoPrecompiler

UNAME := $(shell uname)
LBITS := $(shell getconf LONG_BIT)

ifeq ($(UNAME), Linux)
OS = LINUX
else ifeq ($(UNAME), Darwin)
OS = MACOSX
else
OS = OTHER
endif

LIB_DIR      = lib
CXX 	=  g++ -std=c++11 -Wall -Wextra -g

ifeq ($(OS),MACOSX)
LIBSSL_DIR = /usr/local/Cellar/openssl/1.0.1j
LIBS       = -lnavajo -L../../$(LIB_DIR) -lz  -L$(LIBSSL_DIR)/lib -lssl -lcrypto  -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy
DEFS            =   -D__darwin__ -D__x86__ -fPIC -fno-common -D_REENTRANT
CXXFLAGS        =  -O3  -Wdeprecated-declarations
else
ifeq ($(LBITS),64)
  LIB_DIR=lib64
endif
LIBS       = -lnavajo -L../../$(LIB_DIR) -lz -lssl -lcrypto -pthread  -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy
DEFS            =  -DLINUX -Wall -Wno-unused -fexceptions -fPIC -D_REENTRANT
CXXFLAGS        =  -O4  -Wdeprecated-declarations
endif


CPPFLAGS	= -I. \
            -I$(LIBSSL_DIR)/include \
		  	-I../../include \
			-I/usr/include/opencv -I/usr/include/opencv2
LD		=  g++ 

LDFLAGS        = -Wall -Wno-unused -O3   

MAIN_NAME     = main

MAIN_OBJS = \
		  ConnectedComponent.o \
		  Font.o \
		  Character.o \
		  Line.o \
		  Image.o \
		  Session.o \
		  Binarization.o \
		  GrayscaleCharsDegradationModel.o \
		  ShadowBinding.o \
		  connectedcomponentextraction.o \
		  main.o \

CONNECTEDCOMPONENT= ConnectedComponent

FONT= Font

CHARACTER= Character

LINE = Line

IMAGE= Image

SESSION= Session

BINARIZATION= Binarization

GRAYSCALECHARSDEGRADATIONMODEL= GrayscaleCharsDegradationModel

CONNECTEDCOMPONENTEXTRACTION= connectedcomponentextraction

SHADOWBINDIN= ShadowBinding



#######################
# DEPENDENCE'S RULES  #
#######################

%.o: src/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(CPPFLAGS) $(DEFS) 

all: $(MAIN_NAME)

$(MAIN_NAME): $(MAIN_OBJS) $(LIB_STATIC_NAME)
	rm -f $@
	$(LD) $(LDFLAGS) -o $@ $(MAIN_OBJS) $(LIB_STATIC_NAME) $(LIBS) 


$(IMAGE): $(MAIN_OBJS) $(LIB_STATIC_NAME)
	rm -f $@
	$(LD) $(LDFLAGS) -o $@ $(MAIN_OBJS) $(LIB_STATIC_NAME) $(LIBS) 

$(CONNECTEDCOMPONENT): $(MAIN_OBJS) $(LIB_STATIC_NAME)
	rm -f $@
	$(LD) $(LDFLAGS) -o $@ $(MAIN_OBJS) $(LIB_STATIC_NAME) $(LIBS) 

$(FONT): $(MAIN_OBJS) $(LIB_STATIC_NAME)
	rm -f $@
	$(LD) $(LDFLAGS) -o $@ $(MAIN_OBJS) $(LIB_STATIC_NAME) $(LIBS) 

$(CHARACTER): $(MAIN_OBJS) $(LIB_STATIC_NAME)
	rm -f $@
	$(LD) $(LDFLAGS) -o $@ $(MAIN_OBJS) $(LIB_STATIC_NAME) $(LIBS) 

$(LINE): $(MAIN_OBJS) $(LIB_STATIC_NAME)
	rm -f $@
	$(LD) $(LDFLAGS) -o $@ $(MAIN_OBJS) $(LIB_STATIC_NAME) $(LIBS) 

$(SESSION): $(MAIN_OBJS) $(LIB_STATIC_NAME)
	rm -f $@
	$(LD) $(LDFLAGS) -o $@ $(MAIN_OBJS) $(LIB_STATIC_NAME) $(LIBS)

$(BINARIZATION): $(MAIN_OBJS) $(LIB_STATIC_NAME)
	rm -f $@
	$(LD) $(LDFLAGS) -o $@ $(MAIN_OBJS) $(LIB_STATIC_NAME) $(LIBS)

$(GRAYSCALECHARSDEGRADATIONMODEL): $(MAIN_OBJS) $(LIB_STATIC_NAME)
	rm -f $@
	$(LD) $(LDFLAGS) -o $@ $(MAIN_OBJS) $(LIB_STATIC_NAME) $(LIBS)

$(CONNECTEDCOMPONENTEXTRACTION): $(MAIN_OBJS) $(LIB_STATIC_NAME)
	rm -f $@
	$(LD) $(LDFLAGS) -o $@ $(MAIN_OBJS) $(LIB_STATIC_NAME) $(LIBS)

$(SHADOWBINDING): $(MAIN_OBJS) $(LIB_STATIC_NAME)
	rm -f $@
	$(LD) $(LDFLAGS) -o $@ $(MAIN_OBJS) $(LIB_STATIC_NAME) $(LIBS)


clean:
	@rm -f $(OUTPUTFILENAME) 
	@for i in $(MAIN_OBJS); do  rm -f $$i ; done






