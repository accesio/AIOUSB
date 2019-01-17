#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# CMAKE file for building Ruby wrapper.
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

MESSAGE(STATUS "Rules for building wrapper scripts")

project( ruby_swig )

# Required that we have BUILD_AIOUSB_SHARED ON


FIND_PACKAGE( SWIG REQUIRED )
INCLUDE(${SWIG_USE_FILE})

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# Ruby
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
FIND_PACKAGE( Ruby )
INCLUDE_DIRECTORIES( ${RUBY_INCLUDE_DIRS} )
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_GNU_SOURCE ")

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/.. )

SET_SOURCE_FILES_PROPERTIES(AIOUSB.i PROPERTIES CPLUSPLUS OFF)
SET_SOURCE_FILES_PROPERTIES(AIOUSB.i PROPERTIES SWIG_FLAGS "-I${AIOUSB_INCLUDE_DIR} -includeall ")
SWIG_ADD_MODULE(AIOUSB ruby AIOUSB.i )
SWIG_LINK_LIBRARIES(AIOUSB ${PERL_LIBRARY} aiousb )

