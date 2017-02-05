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

INCLUDE_DIRECTORIES(${libnavajo_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Gui_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDE_DIRS})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/headers)

SET(SRVR_SRCS
  src/binarization.cpp
  src/BleedThrough.cpp
  src/BlurFilter.cpp
  src/Character.cpp
  src/convertor.cpp
  src/ConnectedComponent.cpp
  src/connectedcomponentextraction.cpp
  src/Font.cpp
  src/GrayscaleCharsDegradationModel.cpp
  src/HoleDegradation.cpp
  src/Image.cpp
  src/Line.cpp
  src/PhantomCharacter.cpp
  src/ShadowBinding.cpp
  src/Session.cpp
  src/main.cpp
  src/LineDetection.cpp
  src/BackgroundReconstruction.cpp
  src/textinpainting.cpp
  src/baseline.cpp
  src/StructureDetection.cpp
  src/OCR.cpp
  )

SET(SRVR_HDRS
  headers/binarization.hpp
  headers/BleedThrough.hpp
  headers/BlurFilter.hpp
  headers/Character.hpp
  headers/convertor.h
  headers/ConnectedComponent.hpp
  headers/connectedcomponentextraction.h
  headers/Font.hpp
  headers/GrayscaleCharsDegradationModel.hpp
  headers/HoleDegradation.hpp
  headers/Image.hpp
  headers/json.hpp
  headers/Line.hpp
  headers/PhantomCharacter.hpp
  headers/ShadowBinding.hpp
  headers/Session.hpp
  headers/LineDetection.hpp
  headers/BackgroundReconstruction.hpp
  headers/textinpainting.hpp
  headers/baseline.hpp
  headers/StructureDetection.hpp
  headers/OCR.hpp
)

ADD_EXECUTABLE(main ${SRVR_SRCS} ${SRVR_HDRS})
TARGET_LINK_LIBRARIES(main  ${Tesseract_LIBRARIES} ${libnavajo_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${OpenCV_LIBS} Qt5::Core Qt5::Gui Qt5::Widgets)
