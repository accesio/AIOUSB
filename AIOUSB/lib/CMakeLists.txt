#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# CMAKE file for building libaiousb. Please note that this
# version is required to be able to build static and 
# shared library versions.
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

MESSAGE(STATUS "Loading rules for build libaiousb")
cmake_minimum_required(VERSION 2.8)

project( aiousblibs )

#
# Options for building shared vs. Static libraries
#
OPTION(BUILD_AIOUSB_SHARED "Build the AIOUSB libraries shared." ON)
OPTION(BUILD_AIOUSBDBG_SHARED "Build the AIOUSB Debug libraries shared." ON)
OPTION(BUILD_AIOUSBCPP_SHARED "Build the AIOUSB C++ libraries shared." ON)
OPTION(BUILD_AIOUSBCPPDBG_SHARED "Build the AIOUSB C++ Debug libraries shared." ON)

IF(BUILD_AIOUSB_SHARED)
  SET(AIOUSB_LIBRARY_TYPE SHARED)
ELSE(BUILD_AIOUSB_SHARED)
  SET(AIOUSB_LIBRARY_TYPE STATIC)
ENDIF(BUILD_AIOUSB_SHARED)

IF(BUILD_AIOUSBDBG_SHARED)
  SET(AIOUSBDBG_LIBRARY_TYPE SHARED)
ELSE(BUILD_AIOUSBDBG_SHARED)
  SET(AIOUSBDBG_LIBRARY_TYPE STATIC)
ENDIF(BUILD_AIOUSBDBG_SHARED)

IF(BUILD_AIOUSBCPP_SHARED)
  SET(AIOUSBCPP_LIBRARY_TYPE SHARED)
ELSE(BUILD_AIOUSBCPP_SHARED)
  SET(AIOUSBCPP_LIBRARY_TYPE STATIC)
ENDIF(BUILD_AIOUSBCPP_SHARED)

IF(BUILD_AIOUSBCPPDBG_SHARED)
  SET(AIOUSBCPPDBG_LIBRARY_TYPE SHARED)
ELSE(BUILD_AIOUSBCPPDBG_SHARED)
  SET(AIOUSBCPPDBG_LIBRARY_TYPE STATIC)
ENDIF(BUILD_AIOUSBCPPDBG_SHARED)

SET( tmp_aiousb_files 
  "${CMAKE_CURRENT_SOURCE_DIR}/AIOBuf.c" 
  "${CMAKE_CURRENT_SOURCE_DIR}/AIOChannelMask.c" 
  "${CMAKE_CURRENT_SOURCE_DIR}/AIOChannelRange.c" 
  "${CMAKE_CURRENT_SOURCE_DIR}/AIOCmd.c" 
  "${CMAKE_CURRENT_SOURCE_DIR}/AIOContinuousBuffer.c" 
  "${CMAKE_CURRENT_SOURCE_DIR}/AIOCountsConverter.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/AIODeviceInfo.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/AIODeviceQuery.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/AIODeviceTable.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/AIOEither.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/AIOFifo.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/AIOList.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/AIOProductTypes.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/AIOPlugNPlay.c" 
  "${CMAKE_CURRENT_SOURCE_DIR}/AIOTuple.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/ADCConfigBlock.c"  
  "${CMAKE_CURRENT_SOURCE_DIR}/AIOUSBDevice.c"  
  "${CMAKE_CURRENT_SOURCE_DIR}/AIOUSB_ADC.c" 
  "${CMAKE_CURRENT_SOURCE_DIR}/AIOUSB_CTR.c" 
  "${CMAKE_CURRENT_SOURCE_DIR}/AIOUSB_Core.c" 
  "${CMAKE_CURRENT_SOURCE_DIR}/AIOUSB_CustomEEPROM.c" 
  "${CMAKE_CURRENT_SOURCE_DIR}/AIOUSB_DAC.c" 
  "${CMAKE_CURRENT_SOURCE_DIR}/AIOUSB_DIO.c" 
  "${CMAKE_CURRENT_SOURCE_DIR}/AIOUSB_Log.c" 
  "${CMAKE_CURRENT_SOURCE_DIR}/AIOUSB_Properties.c" 
  "${CMAKE_CURRENT_SOURCE_DIR}/AIOUSB_USB.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/AIOUSB_WDG.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/DIOBuf.c" 
  "${CMAKE_CURRENT_SOURCE_DIR}/USBDevice.c" 
  "${CMAKE_CURRENT_SOURCE_DIR}/CStringArray.c" 
  "${CMAKE_CURRENT_SOURCE_DIR}/cJSON.c" 
  "${CMAKE_CURRENT_SOURCE_DIR}/AIOCommandLine.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/AIOConfiguration.c"
  )


set( CMAKE_INCLUDE_CURRENT_DIR  true )
add_definitions(-D_GNU_SOURCE)
# Store version into variable

execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --always OUTPUT_VARIABLE ACCESIO_TAG_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE )
# The variable will be used when file is configured
configure_file("AIOUSB_Version.h.in" "AIOUSB_Version.h")



#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# Generate 
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
foreach( tmp_c_file ${tmp_aiousb_files} )
  get_filename_component( c_file ${tmp_c_file} NAME )
  string(REGEX REPLACE "\\.c$" "" file_name ${c_file})
  get_filename_component( dir ${tmp_c_file} PATH ) 
  # SET( ofilename_wo_stem "${dir}/${c_file}" )
  set(ofilename_wo_stem "${CMAKE_CURRENT_BINARY_DIR}/${file_name}" )
  # MESSAGE(STATUS "ofilename_wo_stem: ${ofilename_wo_stem}")
  set(aiousb_var "${ofilename_wo_stem}.c" )
  # MESSAGE(STATUS "aiousb_var: ${aiousb_var}")
  set(aiousbcpp_var "${ofilename_wo_stem}.cpp" )
  set(aiousbdbg_var "${ofilename_wo_stem}.dbg.c" )
  set(aiousbcppdbg_var "${ofilename_wo_stem}.dbg.cpp" )

  # MESSAGE( STATUS "Appending ${aiousb_var}")
  LIST(APPEND aiousb_files ${aiousb_var} )
  LIST(APPEND aiousbcpp_files ${aiousbcpp_var} )
  LIST(APPEND aiousbdbg_files ${aiousbdbg_var} )
  LIST(APPEND aiousbcppdbg_files ${aiousbcppdbg_var} )

  #message(STATUS "Trying file ${aiousbcpp_var}" )
  #message(STATUS "Trying file ${aiousb_var}" )
  add_custom_command( OUTPUT ${aiousb_var} COMMAND ${CMAKE_COMMAND} -E copy_if_different ${tmp_c_file} ${aiousb_var} )
  add_custom_command( OUTPUT ${aiousbdbg_var} COMMAND ${CMAKE_COMMAND} -E copy_if_different ${tmp_c_file} ${aiousbdbg_var} )
  add_custom_command( OUTPUT ${aiousbcpp_var} COMMAND ${CMAKE_COMMAND} -E copy_if_different ${tmp_c_file} ${aiousbcpp_var} )
  add_custom_command( OUTPUT ${aiousbcppdbg_var} COMMAND ${CMAKE_COMMAND} -E copy_if_different ${tmp_c_file} ${aiousbcppdbg_var} )
endforeach( tmp_c_file )




add_custom_target( aiousb_copies DEPENDS ${aiousb_files} )
add_custom_target( aiousbcpp_copies DEPENDS ${aiousbcpp_files} )
add_custom_target( aiousbdbg_copies DEPENDS ${aiousbdbg_files} )
add_custom_target( aiousbcppdbg_copies DEPENDS ${aiousbcppdbg_files} )

add_library( aiousb        ${AIOUSB_LIBRARY_TYPE}       ${aiousb_files} )
add_library( aiousbdbg     ${AIOUSBDBG_LIBRARY_TYPE}    ${aiousbdbg_files} )
add_library( aiousbcpp     ${AIOUSBCPP_LIBRARY_TYPE}    ${aiousbcpp_files}  )
add_library( aiousbcppdbg  ${AIOUSBCPPDBG_LIBRARY_TYPE} ${aiousbcppdbg_files} )

include_directories( "${CMAKE_CURRENT_SOURCE_DIR}" )
# MESSAGE(STATUS "Setting include path to include '${CMAKE_CURRENT_SOURCE_DIR}'")
set( AIOUSB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}  )

get_filename_component(LIBUSB_DIRECTORY ${LIBUSB_1_LIBRARIES} PATH)
link_directories( ${LIBUSB_DIRECTORY} )

SET_TARGET_PROPERTIES( aiousbdbg PROPERTIES COMPILE_FLAGS "-DNDEBUG" )
SET_TARGET_PROPERTIES( aiousbcppdbg PROPERTIES COMPILE_FLAGS "-DNDEBUG" )

GET_TARGET_PROPERTY( FOO aiousbdbg COMPILE_FLAGS  )
MESSAGE(STATUS "Cflags for debug is ${FOO}")


target_link_libraries( aiousb ${CORELIBS} ${EXTRA_LIBS} -L${LIBUSB_DIRECTORY} )
target_link_libraries( aiousbcpp ${CORELIBS} ${EXTRA_LIBS}  -L${LIBUSB_DIRECTORY} )
target_link_libraries( aiousbcppdbg ${CORELIBS} ${EXTRA_LIBS}  -L${LIBUSB_DIRECTORY} )
target_link_libraries( aiousbdbg ${CORELIBS} ${EXTRA_LIBS}  -L${LIBUSB_DIRECTORY} )

add_dependencies( aiousb aiousb_copies )
add_dependencies( aiousbcpp aiousbcpp_copies )
add_dependencies( aiousbcppdbg aiousbcppdbg_copies )
add_dependencies( aiousbdbg aiousbdbg_copies )


#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# Testing targets
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
INCLUDE( build_samples )

if( NOT CYGWIN )
endif( NOT CYGWIN )


find_package( GTest ) 
find_package( GMock ) 
find_package( GTestTap )


#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# Built in tests
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
if( GTESTTAP_FOUND AND GMOCK_FOUND AND GTEST_FOUND AND NOT DISABLE_TESTING )

  set(GTEST_FILES ADCConfigBlock.c AIOChannelMask.c AIOChannelRange.c AIOContinuousBuffer.c AIODeviceInfo.c AIODeviceTable.c AIOUSBDevice.c AIOUSB_Core.c DIOBuf.c AIOUSB_DIO.c USBDevice.c AIOFifo.c AIOEither.c AIOCountsConverter.c AIODeviceQuery.c AIOCommandLine.c AIOProductTypes.c AIOTuple.c CStringArray.c AIOList.c )
  foreach( gtest ${GTEST_FILES} ) 
    set(MY_FLAGS "${CXX_FLAGS} -DSELF_TEST -D__aiousb_cplusplus -std=gnu++0x"  )
    set(MY_LIBRARIES aiousbdbg aiousbcpp usb-1.0 pthread m ${GMOCK_BOTH_LIBRARIES} ${GTEST_BOTH_LIBRARIES}  )
    # MESSAGE(STATUS "Libraries: ${MY_LIBRARIES}")
    # set(MY_FLAGS "${MY_FLAGS} -DNDEBUG" )
    build_gtest_cpp_file( aiousblibs ${gtest}  ${MY_FLAGS} "${MY_LIBRARIES}"  )
  endforeach(gtest ${GTEST_FILES} ) 

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# Testing dir tests
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
  FILE( GLOB GTESTING_FILES tests/*.cpp )
  foreach( gtest_file ${GTESTING_FILES} ) 
    # MESSAGE(STATUS "Trying out file ${gtest_file}")
    set(MY_LIBRARIES aiousbcppdbg usb-1.0 pthread m ${GMOCK_BOTH_LIBRARIES} ${GTEST_BOTH_LIBRARIES}  )
    # MESSAGE(STATUS "MY_LIBRARIES=  ${MY_LIBRARIES}" )
    build_gtest_test_file( aiousblibs ${gtest_file} ${MY_FLAGS} "${MY_LIBRARIES}" )
  endforeach( gtest_file )
endif( GTESTTAP_FOUND AND GMOCK_FOUND AND GTEST_FOUND AND NOT DISABLE_TESTING )


if( BUILD_MATLAB AND BUILD_AIOUSB_SHARED ) 
  MESSAGE(STATUS "Building Matlab")
  FIND_PACKAGE(Matlab REQUIRED )
  SET(MEX_COMMAND ${MATLAB_ROOT}/bin/mex -I${LIBUSB_1_INCLUDE_DIRS} -lm -output libaiousb ${tmp_aiousb_files} -L${LIBUSB_DIRECTORY} -lusb-1.0 )
  
  ADD_CUSTOM_COMMAND( OUTPUT libaiousb.mexglx
    COMMAND ${MEX_COMMAND}
    DEPENDS aiousb
    )

  ADD_CUSTOM_TARGET( build_matlab_test ALL 
    DEPENDS libaiousb.mexglx
    COMMENT "Building Matlab library"
    )
  install( FILES ${CMAKE_CURRENT_BINARY_DIR}/libaiousb.mexglx DESTINATION lib/aiousb )
  install( FILES ${CMAKE_CURRENT_SOURCE_DIR}/libaiousb.h DESTINATION include/aiousb )

ELSEIF( BUILD_MATLAB )
  MESSAGE(FATAL_ERROR "You cannot build Matlab because you have elected to not build the shared version of the aiousb library")
endif( BUILD_MATLAB AND BUILD_AIOUSB_SHARED)


#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# Installation targets
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

install( TARGETS aiousb DESTINATION lib )
install( TARGETS aiousbdbg DESTINATION lib )
install( TARGETS aiousbcpp DESTINATION lib )
install( TARGETS aiousbcppdbg DESTINATION lib )
FILE( GLOB aiousb_header_files *.h )
install( FILES ${aiousb_header_files} DESTINATION include/aiousb/ )


