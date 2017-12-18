########################################################
# cmake file for building MarlinTrkProcessors
# - based on $MARLIN/example/mymarlin/CMakeLists.txt
#   by @author Jan Engels, Desy IT
CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)
########################################################


# project name
PROJECT( validateSinglePhotonDST CXX )

# project version
SET( ${PROJECT_NAME}_VERSION_MAJOR 0 )
SET( ${PROJECT_NAME}_VERSION_MINOR 1 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )

cmake_policy(SET CMP0008 NEW)

### DEPENDENCIES ############################################################

FIND_PACKAGE( ILCUTIL REQUIRED COMPONENTS ILCSOFT_CMAKE_MODULES )

# load default settings from ILCSOFT_CMAKE_MODULES
INCLUDE( ilcsoft_default_settings )

FIND_PACKAGE( Marlin 1.0 REQUIRED ) # minimum required Marlin version
INCLUDE_DIRECTORIES( ${Marlin_INCLUDE_DIRS} )
LINK_LIBRARIES( ${Marlin_LIBRARIES} )
ADD_DEFINITIONS( ${Marlin_DEFINITIONS} )

# FIND_PACKAGE( MarlinUtil 1.0 REQUIRED ) # minimum required MarlinUtil version
# INCLUDE_DIRECTORIES( ${MarlinUtil_INCLUDE_DIRS} )
# LINK_LIBRARIES( ${MarlinUtil_LIBRARIES} )
# ADD_DEFINITIONS( ${MarlinUtil_DEFINITIONS} )

FIND_PACKAGE( ROOT REQUIRED ) 
INCLUDE_DIRECTORIES( ${ROOT_INCLUDE_DIRS} )
LINK_LIBRARIES( ${ROOT_LIBRARIES} )
ADD_DEFINITIONS( ${ROOT_DEFINITIONS} )

# FIND_PACKAGE( DD4hep COMPONENTS DDRec )
# INCLUDE_DIRECTORIES( ${DD4hep_INCLUDE_DIRS} )
# LINK_LIBRARIES( ${DD4hep_LIBRARIES} ${DD4hep_COMPONENT_LIBRARIES} )

### DOCUMENTATION ###########################################################

OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )

IF( INSTALL_DOC AND EXISTS "${PROJECT_SOURCE_DIR}/doc/CMakeLists.txt" )
    ADD_SUBDIRECTORY( ./doc )
ENDIF()


### LIBRARY #################################################################

# definitions to pass to the compiler
ADD_DEFINITIONS( "-Wall -ansi -pedantic" )
ADD_DEFINITIONS( "-Wno-long-long" )
ADD_DEFINITIONS( "-std=c++11" )

# include directories
INCLUDE_DIRECTORIES( ./include )

# add library
FILE( GLOB_RECURSE library_sources "src/*.cc" )
# needed for adding header files to xcode project
IF(CMAKE_GENERATOR MATCHES "Xcode")
  FILE( GLOB_RECURSE library_headers "*.h" )
  ADD_SHARED_LIBRARY( ${PROJECT_NAME} ${library_sources} ${library_headers})
ELSE()
  ADD_SHARED_LIBRARY( ${PROJECT_NAME} ${library_sources} )
ENDIF()  

INSTALL_SHARED_LIBRARY( ${PROJECT_NAME} DESTINATION lib )

# display some variables and write them to cache
DISPLAY_STD_VARIABLES()

