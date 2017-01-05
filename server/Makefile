##############################
# EXAMPLE 2                  #
# created by T.DESCOMBES     #
#                    2015    #
##############################

OUTPUTFILENAME = PrecompiledRepository.cc

NAVAJO_PREFIX=/usr
NAVAJO_INCLUDE_DIR=${NAVAJO_PREFIX}/include
NAVAJO_PRECOMPILER_EXEC=${NAVAJO_PREFIX}/bin/navajoPrecompiler


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


#TODO: Use a proper build system !!!...
#TODO: Check that it is the same paths on Debian (OS of the IUT server).
#      BUT IT WILL NOT BUILD ON MACOSX !!!
ifeq ($(OS),LINUX)

#General case (works on Fedora)
QT_SUFFIX=
QT_LIB_PREFIX=/usr/lib64

ifeq ($(wildcard /etc/ubuntu-release),"") #Ubuntu case
QT_SUFFIX=x86_64-linux-gnu/
QT_LIB_PREFIX=/usr/lib/x86_64-linux-gnu
endif

QT_CXXFLAGS=-isystem /usr/include/$(QT_SUFFIX)qt5 -isystem /usr/include/$(QT_SUFFIX)qt5/QtWidgets -isystem /usr/include/$(QT_SUFFIX)qt5/QtGui -isystem /usr/include/$(QT_SUFFIX)qt5/QtCore -isystem $(QT_LIB_PREFIX)/qt5/mkspecs/linux-g++-64 

QT_LIBS=$(QT_LIB_PREFIX)/libQt5Widgets.so $(QT_LIB_PREFIX)/libQt5Gui.so $(QT_LIB_PREFIX)/libQt5Core.so

endif


ifeq ($(OS),MACOSX)

#TODO: set path for Qt
QT_CXXFLAGS=
QT_LIBS=

LIBSSL_DIR = /usr/local/Cellar/openssl/1.0.1j
LIBS       = -lnavajo -L../../$(LIB_DIR) -lz  -L$(LIBSSL_DIR)/lib -lssl -lcrypto  -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy $(QT_LIBS)
DEFS            =   -D__darwin__ -D__x86__ -fPIC -fno-common -D_REENTRANT
CXXFLAGS        =  -O3  -Wdeprecated-declarations $(QT_CXXFLAGS)

else
#Linux

ifeq ($(LBITS),64)
  LIB_DIR=lib64
endif
LIBS       = -lnavajo -L../../$(LIB_DIR) -lz -lssl -lcrypto -pthread  -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_contrib -lopencv_legacy $(QT_LIBS)
DEFS            =  -DLINUX -Wall -Wno-unused -fexceptions -fPIC -D_REENTRANT
CXXFLAGS        =  -O4  -Wdeprecated-declarations $(QT_CXXFLAGS)
endif


CPPFLAGS	= -I. \
            -I$(LIBSSL_DIR)/include \
		  	-Iheaders \
		  	-I../../include \
			-I/usr/include/opencv -I/usr/include/opencv2 -I$(NAVAJO_INCLUDE_DIR)
LD		=  g++ 

LDFLAGS        = -Wall -Wno-unused -O3   

FOLDER_FILES_OBJECT= files_object

MAIN_NAME     = main

MAIN_OBJS = \
		  ConnectedComponent.o \
		  Font.o \
		  Character.o \
		  Line.o \
		  Image.o \
		  Session.o \
		  binarization.o \
		  GrayscaleCharsDegradationModel.o \
		  ShadowBinding.o \
		  PhantomCharacter.o \
		  convertor.o \
		  BlurFilter.o \
		  BleedThrough.o \
		  connectedcomponentextraction.o \
		  main.o \

CONNECTEDCOMPONENT= ConnectedComponent

FONT= Font

CHARACTER= Character

LINE = Line

IMAGE= Image

SESSION= Session

BINARIZATION= binarization

GRAYSCALECHARSDEGRADATIONMODEL= GrayscaleCharsDegradationModel

CONNECTEDCOMPONENTEXTRACTION= connectedcomponentextraction

SHADOWBINDING= ShadowBinding

PHANTOMCHARACTER= PhantomCharacter

CONVERTOR= convertor

BLURFILTER= BlurFilter

BLEEDTHROUGH= BleedThrough

#######################
# DEPENDENCE'S RULES  #
#######################

%.o: src/%.cpp
	$(CXX) -c $< -o $@ $(CXXFLAGS) $(CPPFLAGS) $(DEFS) 

all: $(MAIN_NAME)

$(MAIN_NAME): $(MAIN_OBJS) $(LIB_STATIC_NAME)
	@rm -f $@
	$(LD) $(LDFLAGS) -o $@ $(MAIN_OBJS) $(LIB_STATIC_NAME) $(LIBS)
	@rm -rf $(FOLDER_FILES_OBJECT)
	@mkdir $(FOLDER_FILES_OBJECT)
	@mv *.o $(FOLDER_FILES_OBJECT)


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

$(SHADOWBINDING): $(MAIN_OBJS) $(LIB_STATIC_NAME)
	rm -f $@
	$(LD) $(LDFLAGS) -o $@ $(MAIN_OBJS) $(LIB_STATIC_NAME) $(LIBS)

$(PHANTOMCHARACTER): $(MAIN_OBJS) $(LIB_STATIC_NAME)
	rm -f $@
	$(LD) $(LDFLAGS) -o $@ $(MAIN_OBJS) $(LIB_STATIC_NAME) $(LIBS)

$(CONNECTEDCOMPONENTEXTRACTION): $(MAIN_OBJS) $(LIB_STATIC_NAME)
	rm -f $@
	$(LD) $(LDFLAGS) -o $@ $(MAIN_OBJS) $(LIB_STATIC_NAME) $(LIBS)

$(CONVERTOR): $(MAIN_OBJS) $(LIB_STATIC_NAME)
	rm -f $@
	$(LD) $(LDFLAGS) -o $@ $(MAIN_OBJS) $(LIB_STATIC_NAME) $(LIBS)

$(BLURFILTER): $(MAIN_OBJS) $(LIB_STATIC_NAME)
	rm -f $@
	$(LD) $(LDFLAGS) -o $@ $(MAIN_OBJS) $(LIB_STATIC_NAME) $(LIBS)

$(BLEEDTHROUGH): $(MAIN_OBJS) $(LIB_STATIC_NAME)
	rm -f $@
	$(LD) $(LDFLAGS) -o $@ $(MAIN_OBJS) $(LIB_STATIC_NAME) $(LIBS)


clean:
	@rm -rf $(FOLDER_FILES_OBJECT)
	@rm -f $(MAIN_NAME)
	@for i in $(MAIN_OBJS); do  rm -f $$i ; done






