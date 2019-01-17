#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# CMAKE file for building libaiousb. Please note that this
# version is required to be able to build static and 
# shared library versions.
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

MESSAGE(STATUS "Rules for building wrapper scripts")

project( aiousblibs )

# Required that we have BUILD_AIOUSB_SHARED ON

FIND_PACKAGE( SWIG REQUIRED )
INCLUDE(${SWIG_USE_FILE})


#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# Python
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
FIND_PACKAGE( PythonLibs )
INCLUDE_DIRECTORIES( ${PYTHON_INCLUDE_PATH} )
SET_SOURCE_FILES_PROPERTIES(AIOUSB.i PROPERTIES CPLUSPLUS OFF)
SET_SOURCE_FILES_PROPERTIES(AIOUSB.i PROPERTIES SWIG_FLAGS "-I${AIOUSB_INCLUDE_DIR} -includeall")
SWIG_ADD_MODULE(AIOUSB python python/AIOUSB.i )
SWIG_LINK_LIBRARIES(AIOUSB ${PYTHON_LIBRARIES} aiousb )

#
# Perl
#
#FIND_PACKAGE( PythonLibs )
#INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
#SET_SOURCE_FILES_PROPERTIES(AIOUSB.i PROPERTIES CPLUSPLUS OFF)
#SET_SOURCE_FILES_PROPERTIES(AIOUSB.i PROPERTIES SWIG_FLAGS "-I${AIOUSB_INCLUDE_DIR} -includeall")
#SWIG_ADD_MODULE(AIOUSB python AIOUSB.i )
#SWIG_LINK_LIBRARIES(AIOUSB ${PYTHON_LIBRARIES} aiousb )

#
# Ruby
#

#
# Java
#


#
# Octave
#

#
#
#




#include_directories( "${C_SOURCE_DIR}" )
#set( AIOUSB_LIBS ${C_SOURCE_DIR} )
#
#target_link_libraries( aiousb ${CORELIBS} ${EXTRA_LIBS} )
#add_dependencies( aiousbcpp aiousbcpp_copies )
#add_dependencies( aiousbcppdbg aiousbcppdbg_copies )
#add_dependencies( aiousbdbg aiousbdbg_copies )

