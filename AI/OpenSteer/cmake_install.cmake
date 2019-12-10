# Install script for directory: /run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/OpenSteer" TYPE FILE FILES
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/AbstractVehicle.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/Annotation.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/Camera.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/Clock.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/Color.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/Draw.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/LocalSpace.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/lq.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/Obstacle.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/OldPathway.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/OpenSteerDemo.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/Path.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/Pathway.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/PlugIn.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/PolylineSegmentedPath.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/PolylineSegmentedPathwaySegmentRadii.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/PolylineSegmentedPathwaySingleRadius.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/Proximity.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/QueryPathAlike.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/QueryPathAlikeBaseDataExtractionPolicies.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/QueryPathAlikeMappings.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/QueryPathAlikeUtilities.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/SegmentedPath.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/SegmentedPathAlikeUtilities.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/SegmentedPathway.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/SharedPointer.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/SimpleVehicle.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/StandardTypes.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/SteerLibrary.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/UnusedParameter.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/Utilities.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/Vec3.h"
    "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/include/OpenSteer/Vec3Utilities.h"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/third-party/glfw/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/games/Luna/EMLibs/OpenSteer/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
