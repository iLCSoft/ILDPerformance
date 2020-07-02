########################################################
# cmake file for building Marlin example Package
# @author Jan Engels, Desy IT
CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)
########################################################


# project name
PROJECT( ILDPerformance )


# project version
SET( ${PROJECT_NAME}_VERSION_MAJOR 1 )
SET( ${PROJECT_NAME}_VERSION_MINOR 8 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )



### DEPENDENCIES ############################################################

FIND_PACKAGE( ILCUTIL REQUIRED COMPONENTS ILCSOFT_CMAKE_MODULES )

# load default settings from ILCSOFT_CMAKE_MODULES
INCLUDE( ilcsoft_default_settings )


FIND_PACKAGE( Marlin 1.0 REQUIRED ) # minimum required Marlin version
INCLUDE_DIRECTORIES( SYSTEM ${Marlin_INCLUDE_DIRS} )
LINK_LIBRARIES( ${Marlin_LIBRARIES} )
ADD_DEFINITIONS( ${Marlin_DEFINITIONS} )

FIND_PACKAGE( MarlinTrk )
INCLUDE_DIRECTORIES( SYSTEM ${MarlinTrk_INCLUDE_DIRS} )
LINK_LIBRARIES( ${MarlinTrk_LIBRARIES} )
ADD_DEFINITIONS( ${MarlinTrk_DEFINITIONS} )

FIND_PACKAGE( aidaTT )
ADD_DEFINITIONS( "-D AIDATT_USE_DD4HEP -DUSE_GBL -DUSE_LCIO" )
INCLUDE_DIRECTORIES( SYSTEM ${aidaTT_INCLUDE_DIRS} )
LINK_LIBRARIES( ${aidaTT_LIBRARIES} )
ADD_DEFINITIONS( ${aidaTT_DEFINITIONS} )

FIND_PACKAGE( GBL REQUIRED)
INCLUDE_DIRECTORIES( SYSTEM ${GBL_INCLUDE_DIRS} )
LINK_LIBRARIES( ${GBL_LIBRARIES} )
ADD_DEFINITIONS( ${GBL_DEFINITIONS} )

FIND_PACKAGE( DD4hep )
INCLUDE_DIRECTORIES( SYSTEM ${DD4hep_INCLUDE_DIRS} )
LINK_LIBRARIES( ${DD4hep_LIBRARIES} )
ADD_DEFINITIONS( ${DD4hep_DEFINITIONS} )

FIND_PACKAGE( MarlinUtil 1.0 REQUIRED ) # minimum required MarlinUtil version
INCLUDE_DIRECTORIES( SYSTEM ${MarlinUtil_INCLUDE_DIRS} )
LINK_LIBRARIES( ${MarlinUtil_LIBRARIES} )
ADD_DEFINITIONS( ${MarlinUtil_DEFINITIONS} )

FIND_PACKAGE( ROOT REQUIRED ) 
INCLUDE_DIRECTORIES( SYSTEM ${ROOT_INCLUDE_DIRS} )
LINK_LIBRARIES( ${ROOT_LIBRARIES} )
ADD_DEFINITIONS( ${ROOT_DEFINITIONS} )

find_package(GSL REQUIRED)
INCLUDE_DIRECTORIES( SYSTEM SYSTEM ${GSL_INCLUDE_DIRS} )


### DOCUMENTATION ###########################################################

OPTION( INSTALL_DOC "Set to OFF to skip build/install Documentation" OFF )

IF( INSTALL_DOC AND EXISTS "${PROJECT_SOURCE_DIR}/doc/CMakeLists.txt" )
    ADD_SUBDIRECTORY( ./doc )
ENDIF()



### LIBRARY #################################################################

# definitions to pass to the compiler
#ADD_DEFINITIONS( "-Wall -ansi -pedantic" )
#ADD_DEFINITIONS( "-Wno-long-long" )

# include directories
INCLUDE_DIRECTORIES( 
  ./tracking/include
  ./PID/include  
  ./pi0/include 
  ./validateSim/include 
  ./singlePhotonVal/include 
  ./trackerHitCtr/include 
  ./WWZZSeparation/processor/include
  ./dEdX/include
)
#INSTALL_DIRECTORY( ./include DESTINATION . FILES_MATCHING PATTERN "*.h" )

# add library
AUX_SOURCE_DIRECTORY( ./tracking/src library_sources )
AUX_SOURCE_DIRECTORY( ./PID/src library_sources )
AUX_SOURCE_DIRECTORY( ./pi0/src library_sources )
AUX_SOURCE_DIRECTORY( ./validateSim/src library_sources )
AUX_SOURCE_DIRECTORY( ./singlePhotonVal/src library_sources )
AUX_SOURCE_DIRECTORY( ./trackerHitCtr/src library_sources )
AUX_SOURCE_DIRECTORY( ./WWZZSeparation/processor/src library_sources )
AUX_SOURCE_DIRECTORY( ./dEdX/src library_sources )
ADD_SHARED_LIBRARY( ${PROJECT_NAME} ${library_sources} )
INSTALL_SHARED_LIBRARY( ${PROJECT_NAME} DESTINATION lib )

# add binaries
ADD_EXECUTABLE( ILDPerformance_UdsAnalysis UdsAnalysis/src/ILDPerformance_AnalyseUds.cc )
INSTALL( TARGETS ILDPerformance_UdsAnalysis RUNTIME DESTINATION bin )
ADD_EXECUTABLE( ILDPerformance_dEdXHistPlotter dEdX/main/dEdxHistPlotter.cc )
INSTALL( TARGETS ILDPerformance_dEdXHistPlotter RUNTIME DESTINATION bin )

# display some variables and write them to cache
DISPLAY_STD_VARIABLES()
