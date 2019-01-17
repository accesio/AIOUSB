
macro ( build_sample_c_file project c_file addl_libs ) 
  GET_FILENAME_COMPONENT( tmp_c_file ${c_file} NAME )
  STRING(REGEX REPLACE "\\.c$" "" binary_name ${tmp_c_file})
  ADD_EXECUTABLE( "${project}_${binary_name}" ${c_file} )
  SET_TARGET_PROPERTIES( "${project}_${binary_name}" PROPERTIES OUTPUT_NAME  ${binary_name} ) 
  TARGET_LINK_LIBRARIES( "${project}_${binary_name}" aiousb aiousbcpp aiocommon ${addl_libs} )
  INSTALL(TARGETS "${project}_${binary_name}" DESTINATION "share/accesio/${project}/" ) 
endmacro ( build_sample_c_file) 

macro ( build_sample_cpp_file project cpp_file addl_libs )
  GET_FILENAME_COMPONENT( tmp_cpp_file ${cpp_file} NAME )
  STRING(REGEX REPLACE "\\.cpp$" "" binary_name ${tmp_cpp_file})
  ADD_EXECUTABLE( "${project}_${binary_name}" ${cpp_file} )
  SET_TARGET_PROPERTIES( "${project}_${binary_name}" PROPERTIES OUTPUT_NAME  ${binary_name} ) 
  TARGET_LINK_LIBRARIES( "${project}_${binary_name}" TestCaseSetup ${LIBUSB_1_LIBRARIES} aiousbdbg aiousbcppdbg classaiousbdbg ${addl_libs} )
  INSTALL(TARGETS "${project}_${binary_name}" DESTINATION "share/accesio/${project}/" ) 
endmacro ( build_sample_cpp_file )

#
# Special macro for building testcases that exist inside 
# of the individual C files.
#
macro ( build_selftest_c_file project c_file  cflags link_libraries )
  GET_FILENAME_COMPONENT( tmp_c_file ${c_file} NAME )
  STRING(REGEX REPLACE "\\.c$" "_test" binary_name ${tmp_c_file})
  ADD_EXECUTABLE( "${project}_${binary_name}" ${c_file} )
  LINK_DIRECTORIES( ${AIOUSB_INCLUDE_DIR} )
  if ( USE_GCC)
    SET(MY_CFLAGS " -g -gstabs -std=gnu99 ${cflags}" )
  elseif ( USE_CLANG ) 
    SET(MY_CFLAGS " -g -gstabs ${cflags}" )
  endif( USE_GCC )

  ADD_TEST( NAME ${binary_name} COMMAND ${binary_name} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} )

  SET_TARGET_PROPERTIES( "${project}_${binary_name}" PROPERTIES OUTPUT_NAME  ${binary_name} ) 
  SET_TARGET_PROPERTIES( "${project}_${binary_name}" PROPERTIES COMPILE_FLAGS ${MY_CFLAGS} ) 
  TARGET_LINK_LIBRARIES( "${project}_${binary_name}" ${link_libraries} )
  INSTALL(TARGETS "${project}_${binary_name}" DESTINATION "share/accesio/selftest/${project}" ) 
endmacro ( build_selftest_c_file )

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# Builds an in-file testcase in C++ using GTest 
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
macro ( build_gtest_cpp_file project c_file  cflags link_libraries )
  GET_FILENAME_COMPONENT( tmp_c_file ${c_file} NAME )
  STRING(REGEX REPLACE "\\.c$" "_test" binary_name ${tmp_c_file})
  LINK_DIRECTORIES( ${AIOUSB_INCLUDE_DIR} )
  STRING(REGEX REPLACE "\\.c$" "" file_name ${c_file})
  SET(tmp_gtest_file "${file_name}_gtest.cpp" )
  #MESSAGE(STATUS "Adding GTEST file ${tmp_gtest_file}" )
  ADD_CUSTOM_COMMAND( OUTPUT ${tmp_gtest_file} COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/${c_file} ${tmp_gtest_file} )
  ADD_EXECUTABLE( "${project}_${binary_name}" ${tmp_gtest_file} )

  ADD_TEST( NAME ${binary_name} COMMAND ${binary_name} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} )
  if( MEMORY_PROFILING ) 
    MESSAGE(STATUS "Creating Memcheck command for ${binary_name}")
    add_test(memcheck_${binary_name} ${CTEST_MEMORYCHECK_COMMAND} ${MEMCHECK_OPTIONS} ./${binary_name} )
  endif( MEMORY_PROFILING )
  
  SET_SOURCE_FILES_PROPERTIES( ${tmp_gtest_file}  PROPERTIES LANGUAGE CXX)
  SET_TARGET_PROPERTIES( "${project}_${binary_name}" PROPERTIES OUTPUT_NAME  ${binary_name} ) 
  SET_TARGET_PROPERTIES( "${project}_${binary_name}" PROPERTIES COMPILE_FLAGS ${cflags} ) 
  # MESSAGE(STATUS "Using libs: ${link_libraries}" )
  TARGET_LINK_LIBRARIES( "${project}_${binary_name}" ${link_libraries} )
  INSTALL(TARGETS "${project}_${binary_name}" DESTINATION "share/accesio/selftest/${project}" ) 
endmacro ( build_gtest_cpp_file  )


#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# Builds a Unit test case in the tests directory
# @todo improve by making dependency off of the original file
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
macro ( build_gtest_test_file project cpp_file  cflags link_libraries )
  GET_FILENAME_COMPONENT( just_name_cpp_file ${cpp_file} NAME )
  # MESSAGE("Found file:${cpp_file}")
  LINK_DIRECTORIES( ${AIOUSB_INCLUDE_DIR} )
  #STRING(REGEX REPLACE "\\.c$" "" file_name ${cpp_file})
  #SET(tmp_gtest_file "${tmp_cpp_file}" )
  STRING(REGEX REPLACE "\\.cpp$" "" tmp_gtest_file ${just_name_cpp_file} )
  SET( out_test_file "${CMAKE_CURRENT_BINARY_DIR}/tests/${just_name_cpp_file}" )
  # MESSAGE("Out file: ${out_test_file}")
  #SET( test_target_copied "tests_${tmp_gtest_file}_copied" )
  SET( test_target "tests_${tmp_gtest_file}" )

  # MESSAGE("Copied file: ${out_test_file}")
  # MESSAGE("Custom Target: ${test_target_copied}")
  # MESSAGE("Build Target: ${test_target}")

  ADD_EXECUTABLE( ${test_target} ${cpp_file}  )
  ADD_TEST( NAME ${test_target} COMMAND ${test_target} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} )

  SET_SOURCE_FILES_PROPERTIES( ${out_test_file}  PROPERTIES LANGUAGE CXX)
  SET_TARGET_PROPERTIES( ${test_target} PROPERTIES OUTPUT_NAME ${test_target} ) 
  SET_TARGET_PROPERTIES( ${test_target} PROPERTIES COMPILE_FLAGS ${cflags} ) 

  TARGET_LINK_LIBRARIES( ${test_target} ${link_libraries} )

endmacro ( build_gtest_test_file  )


function ( build_all_samples project ) 
  file( GLOB C_FILES ABSOLUTE "${CMAKE_CURRENT_SOURCE_DIR}/*.c" )
  file( GLOB CXX_FILES ABSOLUTE "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" )
  # MESSAGE(STATUS "Corelibs ${CORELIBS}")
  foreach( c_file ${C_FILES} )
    build_sample_c_file( ${project} ${c_file} ${CORELIBS} )
  endforeach( c_file )
  # SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++0x" )
  # MESSAGE(STATUS "CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}" )
  foreach( cpp_file ${CXX_FILES} )
    build_sample_cpp_file( ${project} ${cpp_file} ${CORELIBS} )
  endforeach( cpp_file )
endfunction ( build_all_samples )

macro ( include_testcase_lib project )
  IF( NOT AIOUSB_INCLUDE_DIR)
    SET(AIOUSB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../lib )
    # MESSAGE(STATUS "Setting AIOUSB_INCLUDE_DIR to be : ${AIOUSB_INCLUDE_DIR}" )
    LINK_DIRECTORIES( ${AIOUSB_INCLUDE_DIR} )
    INCLUDE_DIRECTORIES( ${AIOUSB_INCLUDE_DIR} )
    SET( CMAKE_SHARED_LINKER_FLAGS "-L${AIOUSB_INCLUDE_DIR}" )
  endif(NOT AIOUSB_INCLUDE_DIR )

  if( NOT LIBUSB_FOUND )
    SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../CMakeScripts )
    INCLUDE_DIRECTORIES( ${AIOUSB_INCLUDE_DIR} )
    FIND_PACKAGE(libusb-1.0 REQUIRED )
    INCLUDE_DIRECTORIES( ${LIBUSB_1_INCLUDE_DIRS} )
  endif( NOT LIBUSB_FOUND )

  if( NOT AIOUSB_TESTCASELIB_DIR ) 
    SET( AIOUSB_TESTCASELIB_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/../TestLib )
  endif( NOT AIOUSB_TESTCASELIB_DIR )

  INCLUDE_DIRECTORIES( ${AIOUSB_TESTCASELIB_DIR} )
  LINK_DIRECTORIES( ${AIOUSB_TESTCASELIB_DIR} )  


  if ( USE_GCC AND NOT CYGWIN )
    SET(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -std=gnu99" )
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++0x " )
  endif( USE_GCC AND NOT CYGWIN )

endmacro( include_testcase_lib )


