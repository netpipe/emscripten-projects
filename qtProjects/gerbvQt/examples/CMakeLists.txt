cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)

project(gerbvQtexample)
#find_package(GTK2 2.8 COMPONENTS gtk gtkmm)
find_package(PkgConfig REQUIRED)
find_package(Qt5Gui)

pkg_search_module(GERBV REQUIRED libgerbv)
message( STATUS "FOUND GERBV: ${GERBV_FOUND}" )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}/run/media/netpipe/df7f53ef-2a98-4562-a498-7da578dab660/Programs/Electronics/gerbv-2.7.0/src/.libs

include_directories(${PROJECT_SOURCE_DIR}/../gerbvQt/;${GERBV_INCLUDE_DIRS})
file(GLOB sources ${PROJECT_SOURCE_DIR}/../gerbvQt/*.cpp)
file(GLOB headers ${PROJECT_SOURCE_DIR}/../gerbvQt/*.h)

add_executable(gerbvQtexample example.cpp ${sources} ${headers})

target_link_libraries(gerbvQtexample gerbv Qt5::Gui cairo)
