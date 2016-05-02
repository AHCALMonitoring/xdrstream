#################################################
# cmake file for building xdrstream package
# @author Eté Rémi
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 FATAL_ERROR )
#################################################

# project name
PROJECT( xdrstream )

SET( ${PROJECT_NAME}_VERSION_MAJOR 1 )
SET( ${PROJECT_NAME}_VERSION_MINOR 0 )
SET( ${PROJECT_NAME}_VERSION_PATCH 1 )

# --------------- ilcsoft dependencies -----------
LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )

# load default settings
INCLUDE( default_settings )

OPTION( INSTALL_DOC "Whether to build Doxygen documentation" OFF )

IF( INSTALL_DOC )
	ADD_SUBDIRECTORY( doc )	
ENDIF()

###############################
# Generate xdrstreamConfig.h 
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.h.cmake.in"
    "${PROJECT_SOURCE_DIR}/source/include/${PROJECT_NAME}Config.h" @ONLY )

###############################
# include directories
INCLUDE_DIRECTORIES( source/include )

# require proper c++
ADD_DEFINITIONS( "-pedantic -Wunused-value -O2" )
ADD_DEFINITIONS("-Wno-long-long -Wreturn-type")

AUX_SOURCE_DIRECTORY( ./source/src/ SRC_FILES )

ADD_SHARED_LIBRARY( ${PROJECT_NAME} ${SRC_FILES} )

INSTALL( 
    TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
)

INSTALL( DIRECTORY source/include DESTINATION . PATTERN *.h )

###############################
# display some variables and write them to cache
DISPLAY_STD_VARIABLES()

###############################
# generate and install following configuration files
GENERATE_PACKAGE_CONFIGURATION_FILES( ${PROJECT_NAME}Config.cmake ${PROJECT_NAME}ConfigVersion.cmake ${PROJECT_NAME}LibDeps.cmake )

