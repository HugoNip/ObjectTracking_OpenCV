cmake_minimum_required(VERSION 3.16)
project(c09)

set(CMAKE_CXX_STANDARD 11)

# Opencv requires
find_package(OpenCV 4.0.0 REQUIRED)
message("OpenCV version: ${OpenCV_VERSION} ")

include_directories(${OpenCV_INCLUDE_DIRS})
link_directories(${openCV_LIB_DIR})

add_executable(coloredObjectTracker coloredObjectTracker.cpp)
target_link_libraries(coloredObjectTracker ${OpenCV_LIBS})

add_executable(camShiftTracker camShiftTracker.cpp)
target_link_libraries(camShiftTracker ${OpenCV_LIBS})

add_executable(harrisCornersTracker harrisCornersTracker.cpp)
target_link_libraries(harrisCornersTracker ${OpenCV_LIBS})

add_executable(goodFeaturesToTrack goodFeaturesToTrack.cpp)
target_link_libraries(goodFeaturesToTrack ${OpenCV_LIBS})

add_executable(lucasKanadeTracker lucasKanadeTracker.cpp)
target_link_libraries(lucasKanadeTracker ${OpenCV_LIBS})

add_executable(farnebackTracker farnebackTracker.cpp)
target_link_libraries(farnebackTracker ${OpenCV_LIBS})