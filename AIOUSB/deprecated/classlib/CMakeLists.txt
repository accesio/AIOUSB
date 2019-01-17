#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# CMAKE file for building aioclasslib. This library is deprecated
# and will be replaced over time with the SWIG code under lib/wrappers
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
cmake_minimum_required(VERSION 2.8)



if( NOT AIOUSB_INCLUDE_DIR)
  set(AIOUSB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../lib )
  # MESSAGE(STATUS "Setting AIOUSB_INCLUDE_DIR to be : ${AIOUSB_INCLUDE_DIR}" )
  LINK_DIRECTORIES( ${AIOUSB_INCLUDE_DIR} )
  INCLUDE_DIRECTORIES( ${AIOUSB_INCLUDE_DIR} )
  SET( CMAKE_SHARED_LINKER_FLAGS "-L${AIOUSB_INCLUDE_DIR}" )
endif(NOT AIOUSB_INCLUDE_DIR )



project( classaiousb )
MESSAGE(STATUS "Loading rules to build <deprecated> aiousbclasslib ")

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# Options for building shared vs. Static libraries
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
OPTION(BUILD_CLASS_AIOUSB_SHARED "Build the AIOUSB C++ Class libraries shared." ON)
OPTION(BUILD_CLASS_AIOUSBDBG_SHARED "Build the AIOUSB C++ Class libraries shared." ON)

IF(BUILD_CLASS_AIOUSB_SHARED)
  SET(CLASSAIOUSB_LIBRARY_TYPE SHARED)
ELSE(BUILD_CLASS_AIOUSB_SHARED)
  SET(CLASSAIOUSB_LIBRARY_TYPE STATIC)
ENDIF(BUILD_CLASS_AIOUSB_SHARED)

IF(BUILD_CLASS_AIOUSBDBG_SHARED)
  SET(CLASSAIOUSBDBG_LIBRARY_TYPE SHARED)
ELSE(BUILD_CLASS_AIOUSBDBG_SHARED)
  SET(CLASSAIOUSBDBG_LIBRARY_TYPE STATIC) 
ENDIF(BUILD_CLASS_AIOUSBDBG_SHARED)

file (GLOB classaiousb_files  ABSOLUTE "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" )
file (GLOB hppfiles ABSOLUTE "${CMAKE_CURRENT_SOURCE_DIR}/*.h.*" )

# MESSAGE(STATUS "All files are :${hppfiles}, " )

get_directory_property( output INCLUDE_DIRECTORIES )


#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# Generate 
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

foreach( tmp_hpp_file ${hppfiles} )
  GET_FILENAME_COMPONENT( hpp_file ${tmp_hpp_file} NAME )
  STRING(REGEX REPLACE "\\.h.*$" "" file_name ${hpp_file})
  GET_FILENAME_COMPONENT( dir ${tmp_hpp_file} PATH ) 

  SET(ofilename_wo_stem "${CMAKE_CURRENT_BINARY_DIR}/${file_name}" )
  SET(hppaiousb_var "${ofilename_wo_stem}.hpp" )

  LIST(APPEND hppaiousb_files ${hppaiousb_var} )

  ADD_CUSTOM_COMMAND( OUTPUT ${hppaiousb_var} COMMAND ${CMAKE_COMMAND} -E copy_if_different ${tmp_hpp_file} ${hppaiousb_var} )
endforeach( tmp_hpp_file )

foreach( tmp_cpp_file ${classaiousb_files} )
  get_filename_component( cpp_file ${tmp_cpp_file} NAME )
  string(REGEX REPLACE "\\.cpp$" "" file_name ${cpp_file})
  get_filename_component( dir ${tmp_cpp_file} PATH ) 
  
  set(ofilename_wo_stem "${CMAKE_CURRENT_BINARY_DIR}/${file_name}" )
  set(classaiousbcpp_var "${ofilename_wo_stem}.cpp" )
  set(classaiousbcppdbg_var "${ofilename_wo_stem}.dbg.cpp" )

  LIST(APPEND classaiousbcpp_files ${classaiousbcpp_var} )
  LIST(APPEND classaiousbcppdbg_files ${classaiousbcppdbg_var} )


  add_custom_command( OUTPUT ${classaiousbcpp_var} COMMAND ${CMAKE_COMMAND} -E copy_if_different ${tmp_cpp_file} ${classaiousbcpp_var} )
  add_custom_command( OUTPUT ${classaiousbcppdbg_var} COMMAND ${CMAKE_COMMAND} -E copy_if_different ${tmp_cpp_file} ${classaiousbcppdbg_var} )

endforeach( tmp_cpp_file ${classaiousb_files} )

include_directories( ${AIOUSB_INCLUDE_DIR} )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )

add_custom_target( hppaiousb_copies DEPENDS ${hppaiousb_files} )
add_custom_target( classaiousbcpp_copies DEPENDS ${classaiousbcpp_files} )
add_custom_target( classaiousbcppdbg_copies DEPENDS ${classaiousbcppdbg_files} )


# Requires that you have build the aiousb library first
add_library( classaiousb ${CLASSAIOUSB_LIBRARY_TYPE}  ${classaiousbcpp_files} )
add_library( classaiousbdbg ${CLASSAIOUSBDBG_LIBRARY_TYPE} ${classaiousbcppdbg_files} )

add_dependencies( classaiousb  hppaiousb_copies classaiousbcpp_copies classaiousbcppdbg_copies )
add_dependencies( classaiousbdbg hppaiousb_copies classaiousbcpp_copies classaiousbcppdbg_copies )

target_link_libraries( classaiousb ${CORELIBS} ${EXTRA_LIBS} aiousb aiousbcpp )
target_link_libraries( classaiousbdbg ${CORELIBS} ${EXTRA_LIBS} aiousbdbg aiousbcppdbg )

#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
# Installation targets
#=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

install( TARGETS classaiousb DESTINATION lib )
install( TARGETS classaiousbdbg DESTINATION lib )


