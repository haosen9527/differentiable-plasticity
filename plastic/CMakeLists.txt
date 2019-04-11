cmake_minimum_required(VERSION 2.8.3)
project(plastic)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11 -O2)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  network
  tools
  data
)

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if your package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
  INCLUDE_DIRS include
  LIBRARIES plastic
#  CATKIN_DEPENDS roscpp
#  DEPENDS system_lib
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations

## Declare a C++ library
#add_library(${PROJECT_NAME}
#  src/plastictrain.cpp
#)

include_directories(
  include
  /usr/local/include/eigen3
  ~/tensorflow
  ~/tensorflow/bazel-genfiles
  ~/tensorflow/tensorflow/contrib/makefile/gen/protobuf/include
  ${network_INCLUDE_DIRS}
  ${tools_INCLUDE_DIRS}
  ${data_INCLUDE_DIRS}
  ${catkin_INCLUDE_DIRS}
)

link_directories("~/tensorflow/bazel-bin/tensorflow")

############
## output ##
############
add_executable(${PROJECT_NAME}_train
  src/plastictrain.cpp
  src/train.cpp
  src/label.cpp
)
target_link_libraries(${PROJECT_NAME}_train
  ${catkin_LIBRARIES}
  data
  network
  tensorflow_cc
  tensorflow_framework
)
add_executable(${PROJECT_NAME}_test
  src/plastictrain.cpp
  src/plastictest.cpp
  src/label.cpp
)
target_link_libraries(${PROJECT_NAME}_test
  ${catkin_LIBRARIES}
  data
  network
  tensorflow_cc
  tensorflow_framework
)

#############
## Install ##
#############

#############
## Testing ##
#############
