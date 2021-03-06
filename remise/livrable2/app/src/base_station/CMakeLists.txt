cmake_minimum_required(VERSION 2.8.3)
project(base_station)

find_package(catkin REQUIRED COMPONENTS roscpp rospy std_msgs message_generation kinocto)
find_package(OpenCV REQUIRED)
find_package(Qt4 REQUIRED)

#######################################
## Declare ROS messages and services ##
#######################################

# add_message_files(
#   FILES
#   Num.msg
# )

# add_service_files(
#   FILES
#   AddTwoInts.srv
# )

# generate_messages(
#   DEPENDENCIES
#   std_msgs
# )

###################################################
## Declare things to be passed to other projects ##
###################################################

## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
   INCLUDE_DIRS include
#  LIBRARIES base_station
   CATKIN_DEPENDS roscpp rospy std_msgs message_runtime
#  DEPENDS system_lib
)

###########
## Build ##
###########
add_definitions(${QT_DEFINITIONS})

set(PROJ_SOURCES)
file(GLOB_RECURSE PROJ_SOURCES src/*.cpp)

include_directories(include ${catkin_INCLUDE_DIRS} ${QT_INCLUDES})

qt4_wrap_cpp(PROJ_SOURCES include/MainWindow.h)

add_executable(BaseStation ${PROJ_SOURCES})
add_dependencies(BaseStation kinocto_gencpp)
target_link_libraries(BaseStation ${catkin_LIBRARIES} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${OpenCV_LIBS})

#############
## Install ##
#############

## Mark executable scripts (Python etc.) for installation
## not required for python when using catkin_python_setup()
# install(PROGRAMS
#   scripts/my_python_script
#   DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )

## Mark executables and/or libraries for installation
# install(TARGETS base_station base_station_node
#   ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
#   RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )

## Mark cpp header files for installation
# install(DIRECTORY include/${PROJECT_NAME}/
#   DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
#   FILES_MATCHING PATTERN "*.h"
#   PATTERN ".svn" EXCLUDE
# )

## Mark other files for installation (e.g. launch and bag files, etc.)
# install(FILES
#   # myfile1
#   # myfile2
#   DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
# )

#############
## Testing ##
#############

catkin_add_gtest(baseStationTest test/AllTests.cpp src/Kinect.cpp src/Utility.cpp)
target_link_libraries(baseStationTest ${catkin_LIBRARIES} ${OpenCV_LIBS})
