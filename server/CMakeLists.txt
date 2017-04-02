cmake_minimum_required(VERSION 2.8.11)

PROJECT(server)


## allow more human readable "if then else" constructs
SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS TRUE)

## Set build_type if not set
IF (NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
ENDIF()

## Add more warnings
IF(CMAKE_COMPILER_IS_GNUCC) #may be GCC or clang
  # With GCC, always set flags to have most warnings
  ADD_DEFINITIONS(-Wall -Wextra) # -ansi -pedantic

  ADD_DEFINITIONS(-D_REENTRANT) #was in original makefile
ENDIF()

#C++11 (for all executables)
IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  EXECUTE_PROCESS(COMMAND "${CMAKE_CXX_COMPILER} -dumpversion" OUTPUT_VARIABLE GCC_VERSION)
  IF (GCC_VERSION VERSION_LESS 4.7)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
  ELSE()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  ENDIF()
ELSEIF ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  IF(APPLE)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
  ELSE()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  ENDIF()
ENDIF()


SET(CMAKE_MODULE_PATH  "${CMAKE_SOURCE_DIR}/cmake/Modules;${CMAKE_MODULE_PATH}" )


FIND_PACKAGE(Libnavajo REQUIRED)
FIND_PACKAGE(Threads) #For Libnavajo
FIND_PACKAGE(Qt5Core REQUIRED)
FIND_PACKAGE(Qt5Gui REQUIRED)
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(OpenCV REQUIRED)
FIND_PACKAGE(Tesseract REQUIRED)

#FIND_PACKAGE(OpenSSL REQUIRED)


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/headers/degradations
  ${CMAKE_CURRENT_SOURCE_DIR}/headers/manualFontExtractor
  ${CMAKE_CURRENT_SOURCE_DIR}/headers/synthetize
  ${CMAKE_CURRENT_SOURCE_DIR}/headers/util
  ${CMAKE_CURRENT_SOURCE_DIR}/headers/webPages)

SET(SRVR_SRCS
  src/main.cpp

  src/degradations/BleedThrough.cpp
  src/degradations/BlurFilter.cpp
  src/degradations/GrayscaleCharsDegradationModel.cpp
  src/degradations/PhantomCharacter.cpp
  src/degradations/ShadowBinding.cpp
  src/degradations/degradations.cpp
  src/degradations/HoleDegradation.cpp
  
  src/manualFontExtractor/Character.cpp
  src/manualFontExtractor/ConnectedComponent.cpp
  src/manualFontExtractor/connectedcomponentextraction.cpp
  src/manualFontExtractor/Font.cpp
  src/manualFontExtractor/Line.cpp
  src/manualFontExtractor/Image.cpp
  
  src/util/binarization.cpp
  src/util/convertor.cpp
  src/util/Session.cpp
  src/util/baseline.cpp
  src/util/Config.cpp
  src/util/util.cpp
  
  src/synthetize/LineDetection.cpp
  src/synthetize/BackgroundReconstruction.cpp
  src/synthetize/textinpainting.cpp
  src/synthetize/StructureDetection.cpp
  src/synthetize/OCR.cpp 
  src/synthetize/Painter.cpp
  src/synthetize/SynthetizeImage.cpp
  src/synthetize/synthetizeTest.cpp
  src/synthetize/DocumentXML.cpp
  src/synthetize/LoadLetter.cpp
  
  src/webPages/gestionSession.cpp
  src/webPages/manualFontExtractor.cpp
  src/webPages/StartSession2.cpp
  src/webPages/GetSynthetizeImage.cpp
  )


CONFIGURE_FILE(config.ini config.ini)

ADD_EXECUTABLE(documentCreatorWeb ${SRVR_SRCS})
TARGET_LINK_LIBRARIES(documentCreatorWeb  ${Tesseract_LIBRARIES} ${libnavajo_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${OpenCV_LIBS} Qt5::Core Qt5::Gui Qt5::Widgets)
