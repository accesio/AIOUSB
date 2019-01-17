#
# CMAKE file for building aiousb. Please note that this
# version is required to be able to build static and 
# shared library versions.

message( STATUS "${CMAKE_SOURCE_DIR}" )


set( THIS_PROJECT "OFF" )

option(BUILD_SAMPLES "Build the AIOUSB Samples" ON)
option(BUILD_PERL "Build the Perl Interfaces" OFF)
option(BUILD_PYTHON "Build the Python Interfaces" OFF)
option(BUILD_JAVA "Build the Java Interfaces" OFF)
option(BUILD_R "Build the R Interfaces" OFF)
option(BUILD_OCTAVE "Build the Octave Interfaces" OFF)
option(BUILD_PHP "Build the PHP Interfaces" OFF)
option(BUILD_DOCUMENTATION "Build the Doxygen documentation" OFF )
option(DISABLE_TESTING "Turns off BIST on C/C++ samples" OFF )

SET(FIRMWARE_DIRECTORY /usr/share/usb CACHE PATH "Directory to install .HEX firmware files")
SET(UDEV_DIRECTORY /etc/udev/rules.d CACHE PATH "Directory to install UDEV rules files"  )


option(INSTALL_WRAPPERS_RELATIVE "Install Scripting languages in Paths relative to CMAKE_INSTALL_PREFIX" ON )
option(GUESS_WRAPPER_INSTALL_DIRECTORIES "Guess the location of Wrapper installation paths" ON )
option(MEMORY_PROFILING "Run Memory profiler on tests" OFF )



set(CPACK_COMPONENT_AIOUSBLIB_DISPLAY_NAME "ACCES I/O USB libraries")
set(CPACK_COMPONENT_AIOUSBLIB_DESCRIPTION "USB drivers for the ACCES I/O Products' line of USB data acquisition boards")
set(CPACK_COMPONENT_AIOUSBLIB_GROUP "Foo")

enable_testing()


if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "Prevented in-tree built. Please create a 'build' directory outside of the source code and call 'cmake' or 'ccmake' from there")
endif()

set(CMAKE_LEGACY_CYGWIN_WIN32 0)
cmake_minimum_required(VERSION 2.8.9)


project( AIOUSBLIBS )


# ROOT DIRECTORY FOR INSTALLATION
#set( AIOUSB_INSTALL_PREFIX /usr/share/aiousb )


# use, i.e. don't skip the full RPATH for the build tree
set(CMAKE_SKIP_BUILD_RPATH  FALSE)
# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) 

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
# set(CMAKE_INSTALL_PREFIX "${AIOUSB_INSTALL_PREFIX}" CACHE INTERNAL "Prefix for installation directories" FORCE)


set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeScripts)
set(C_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CPP_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deprecated/classlib)
set(SWIG_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/wrappers )

# Threads are required in both Win and Unix 
# Acquisition libraries
find_package(Threads REQUIRED )
find_package(SWIG )
find_package(Doxygen)
find_package(Perl)
find_package(VALGRIND)
find_package(Git)

if( VALGRIND_PROGRAM AND MEMORY_PROFILING ) 
  set(CTEST_MEMORYCHECK_COMMAND ${VALGRIND_PROGRAM} )
  set(CTEST_MEMORYCHECK_COMMAND_OPTIONS "--error-exitcode=231 -q --tool=memcheck --leak-check=full --show-reachable=yes ")
  set(MEMCHECK_OPTIONS "--error-exitcode=231;--tool=memcheck;--leak-check=full;--show-reachable=yes" )
  #message(STATUS "MEMCheck is ${CTEST_MEMORYCHECK_COMMAND}" )
  #message(STATUS "Options are ${CTEST_MEMORYCHECK_COMMAND_OPTIONS}" )
endif( VALGRIND_PROGRAM AND MEMORY_PROFILING )

if( UNIX OR APPLE OR LINUX )
  find_package(glibc REQUIRED)
  if( GLIBC_VERSION LESS 2.17 ) 
    message(STATUS "$GLIBC_VERSION is less than 2.17, using rt")
    set(CORELIBS ${CORELIBS} rt )
  else() 
  endif( GLIBC_VERSION LESS 2.17 ) 
endif(UNIX OR APPLE OR LINUX ) 

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# Linux / Mac Build 
# 1. Put this into an included file
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
if(  UNIX OR APPLE ) 
  find_package(libusb-1.0 REQUIRED )
  include_directories( ${LIBUSB_1_INCLUDE_DIRS} )
  message( STATUS "Trying Apple / Unix" )

  set(EXTRA_LIBS pthread m)
  if( CMAKE_COMPILER_IS_GNUCC )
    set( USE_GCC TRUE )
  elseif( CMAKE_C_COMPILER_ID MATCHES "Clang")
    set(USE_CLANG TRUE )
  endif( CMAKE_COMPILER_IS_GNUCC ) 

  if(USE_GCC OR USE_CLANG)
    message(STATUS "GCC compiler?:${USE_GCC}")
    if( USE_GCC ) 
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
    endif( USE_GCC )
    
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__aiousb_cplusplus ")
    if (USE_GCC AND NOT CYGWIN )
      set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -fPIC" )
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" )	
    endif(USE_GCC AND NOT CYGWIN )
    
    set(CORELIBS ${CORELIBS} usb-1.0 )
  endif(USE_GCC OR USE_CLANG)

endif( UNIX OR APPLE)


add_subdirectory(lib)
include_directories(lib)
add_subdirectory(deprecated/classlib)
include_directories(deprecated/classlib)


if ( SWIG_FOUND ) 
  if ( BUILD_PYTHON )
    message(STATUS "Adding wrapper for python" )
    add_subdirectory("lib/wrappers/python")
  endif (BUILD_PYTHON)
  if ( BUILD_PERL )
    message(STATUS "Adding wrapper for Perl" )
    add_subdirectory("lib/wrappers/perl")
  endif (BUILD_PERL)
  if ( BUILD_JAVA )
    message(STATUS "Adding wrapper for Java" )
    add_subdirectory("lib/wrappers/java")
  endif (BUILD_JAVA)
  if ( BUILD_RUBY )
    message(STATUS "Adding wrapper for Ruby" )
    add_subdirectory("lib/wrappers/ruby")
  endif (BUILD_RUBY)
  if ( BUILD_OCTAVE )
    message(STATUS "Adding wrapper for Octave" )
    add_subdirectory("lib/wrappers/octave")
  endif (BUILD_OCTAVE )
  if ( BUILD_R )
    message(STATUS "Adding wrapper for R" )
    add_subdirectory("lib/wrappers/R")
  endif (BUILD_R)
  if ( BUILD_PHP )
    message(STATUS "Adding wrapper for PHP" )
    add_subdirectory("lib/wrappers/php")
  endif (BUILD_PHP)
endif( SWIG_FOUND )


add_subdirectory(samples/TestLib)
set( AIOUSB_TESTCASELIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/samples/TestLib )

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# Auto build the samples directory
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

if( BUILD_SAMPLES ) 
  FOREACH( SAMPLE_DIR USB-AO16-16 USB-AI16-16 USB-DA12-8A USB-DIO-16 USB-DIO-32 USB-IIRO-16_8 USB-IDIO-16_8 USB-CTR-15 )
    message(STATUS "Including samples for samples/${SAMPLE_DIR}")
    STRING(REGEX REPLACE  "-" "_" THIS_PROJECT ${SAMPLE_DIR} )
    ADD_SUBDIRECTORY("samples/${SAMPLE_DIR}")
  ENDFOREACH( SAMPLE_DIR  USB-AO16-16 USB-AI16-16 USB-DA12-8A USB-DIO-16 USB-DIO-32  USB-IIRO-16 USB-IDIO-16 USB-IDIO-8 )
endif( BUILD_SAMPLES )


#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# Documentation and man pages
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
if( DOXYGEN_FOUND AND BUILD_DOCUMENTATION )
  message(STATUS "Generating Doxygen Documentation ") 
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

  ADD_CUSTOM_COMMAND(OUTPUT BuildDoxygen.txt 
    COMMAND 
    ${DOXYGEN_EXECUTABLE} Doxyfile
    COMMAND
    ${CMAKE_COMMAND} -E touch BuildDoxygen.txt 
    COMMENT "Generating API documentation with Doxygen"
    DEPENDS aiousb
    )

  if( PERL_FOUND )
    set(PAR "(" )
    set(CPAR ")" )
    set(REPLACE_COMMAND ${PERL_EXECUTABLE} -i -pne 's/^\\s*${PAR}References|Referenced\\s+by${CPAR}.*//g' * )

    ADD_CUSTOM_COMMAND( TARGET BuildDoxygen.txt
      POST_BUILD
      COMMAND ${REPLACE_COMMAND}
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/man/man3
      COMMENT "Cleaning up man pages"
      )
    
    ADD_CUSTOM_TARGET( doc ALL DEPENDS BuildDoxygen.txt
      COMMENT "Building Documentation"
      )

  endif( PERL_FOUND )
  
  set(CPACK_COMPONENT_MANPAGES_DISPLAY_NAME "Man pages")
  set(CPACK_COMPONENT_MANPAGES_DESCRIPTION "Man pages for the API")
  set(CPACK_COMPONENT_MANPAGES_GROUP "docs")

  install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/man/man3 DESTINATION share/accesio/man COMPONENT manpages )
endif( DOXYGEN_FOUND AND BUILD_DOCUMENTATION )

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# CPack stuff
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#set(CPACK_RPM_COMPONENT_INSTALL ON)


set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "ACCES I/O Products USB Drivers")
set(CPACK_PACKAGE_VENDOR "ACCES")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "117")
set(CPACK_PACKAGE_VERSION_PATCH "0" )
set(CPACK_PACKAGE_NAME "libaiousb" )
set(CPACK_PACKAGE_ARCHITECTURE "amd64" )
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "This project contains ACCES I/O Products' USB userspace drivers (based off of libusb)" )
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}\n and samples for ACCES I/O Product's line of USB based " )
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CPACK_PACKAGE_DESCRIPTION_SUMMARY}\n data acquisition modules." )

set(CPACK_PACKAGE_INSTALL_DIRECTORY "accesio ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")

if( WIN32 AND NOT UNIX )        # WIN32
  set(CPACK_NSIS_CONTACT "support@accesio.com")
ELSE( UNIX )                    # LINUX / UNIX 
  set(CPACK_SOURCE_STRIP_FILES "")
  set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Jimi Damon <jdamon@accesio.com>") #required
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${CPACK_PACKAGE_ARCHITECTURE} )
  set(CPACK_RPM_PACKAGE_ARCHITECTURE ${CPACK_PACKAGE_ARCHITECTURE} )
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "libusb-1.0-0 ( >= 1.0.0 ), libusb-dev ( >= 0.1.0 )")
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, cmake ( >= 2.8 ), cmake-curses-gui ( >= 2.8 ) , cmake-data ( >= 2.8 )")
  set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/accesio/AIOUSB" )
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, gcc , binutils")

  # RPM Stuff
  set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
  set(CPACK_RPM_PACKAGE_SUMMARY "This project contains ACCES I/O Products USB userspace drivers based off of libusb")
  set(CPACK_RPM_PACKAGE_LICENSE "${CPACK_RESOURCE_FILE_LICENSE}")
  set(CPACK_RPM_PACKAGE_REQUIRES "libusb-1.0 >= 1.0.0, cmake >= 2.8 , cmake-data >= 2.8")
  set(CPACK_RPM_PACKAGE_SUMMARY "ACCES I/O Products' libusb based userspace drivers" )
  set(CPACK_RPM_PACKAGE_RELEASE	"1.0" )

endif(WIN32 AND NOT UNIX )

file( GLOB HEX_FILES ABSOLUTE "${CMAKE_CURRENT_SOURCE_DIR}/Firmware/*.hex" )
file( GLOB UDEV_RULES ABSOLUTE "${CMAKE_CURRENT_SOURCE_DIR}/Firmware/*.rules" )
file( GLOB PERL_FILES ABSOLUTE "${CMAKE_CURRENT_SOURCE_DIR}/Firmware/accesloader.pl" )

install(FILES ${HEX_FILES} DESTINATION ${FIRMWARE_DIRECTORY} COMPONENT firmware )
install(FILES ${UDEV_RULES} DESTINATION ${UDEV_DIRECTORY} COMPONENT firmware )
install(FILES ${PERL_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin COMPONENT firmware )

set(CPACK_COMPONENT_MANPAGES_GROUP "docs")

set(CPACK_COMPONENTS_ALL Foo manpages )


INCLUDE(CPack)


