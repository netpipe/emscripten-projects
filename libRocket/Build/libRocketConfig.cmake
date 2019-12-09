
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was libRocketConfig.cmake.build.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../../../../usr/local" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

set_and_check(libRocket_INCLUDE_DIRS "/home/netpipe/Desktop/EMScripten-AddonBundle/emscripten-projects/libRocket/Build/../Include")
set(libRocket_LIBRARIES RocketCore;RocketControls;RocketDebugger)
list(GET libRocket_LIBRARIES 0 ROCKET_FIRST_TARGET)

if(NOT (TARGET ${ROCKET_FIRST_TARGET}))
  include("${CMAKE_CURRENT_LIST_DIR}/libRocketTargets.cmake")
endif()

check_required_components(libRocket)
