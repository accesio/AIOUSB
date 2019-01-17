#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# CMAKE file for building Perl wrapper.
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

project( perl_swig )

# Required that we have BUILD_AIOUSB_SHARED ON

FIND_PACKAGE( SWIG REQUIRED )
INCLUDE(${SWIG_USE_FILE})

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# Perl
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-


FIND_PACKAGE( Perl )
FIND_PACKAGE( PerlLibs )
INCLUDE_DIRECTORIES( ${PERL_INCLUDE_PATH} ${PERL_ARCHLIB}/Core )

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_GNU_SOURCE ")

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/.. )
SET_SOURCE_FILES_PROPERTIES(AIOUSB.i PROPERTIES CPLUSPLUS OFF)
SET_SOURCE_FILES_PROPERTIES(AIOUSB.i PROPERTIES SWIG_FLAGS "-I${AIOUSB_INCLUDE_DIR} -includeall ")

SWIG_ADD_MODULE(AIOUSBPerl perl AIOUSB.i )
SWIG_LINK_LIBRARIES(AIOUSBPerl ${PERL_LIBRARY} aiousb )

ADD_CUSTOM_COMMAND( TARGET ${SWIG_MODULE_AIOUSBPerl_REAL_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND}
  ARGS  -E copy "${CMAKE_SHARED_LIBRARY_PREFIX}AIOUSBPerl${CMAKE_SHARED_LIBRARY_SUFFIX}" AIOUSB${CMAKE_SHARED_LIBRARY_SUFFIX}
  COMMENT "Copying Perl files to correct naming convention"
)
#
# Strip off the lib at the beginning
#
STRING(REGEX REPLACE "^lib" "" perl_swig_lib ${SWIG_MODULE_AIOUSBPerl_REAL_NAME})

OPTION( INSTALL_PERL "Build the AIOUSB Samples" ON )


IF( NOT PERL_INSTALL_PREFIX ) 

  SET(PERL_INSTALL_PREFIX "${PERL_ARCHLIB}" CACHE PATH "Location to install Perl libraries"  )

ENDIF( NOT PERL_INSTALL_PREFIX ) 

INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/AIOUSB.so DESTINATION ${PERL_INSTALL_PREFIX}/auto/AIOUSB/  COMPONENT perl_wrapper )
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/AIOUSB.pm DESTINATION ${PERL_INSTALL_PREFIX} COMPONENT perl_wrapper )







