project(test_project)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(CMAKE_CXX_FLAGS -std=c++0x)

set(SlicerExecutionModel_DIR /work/boucaud/SEM/SlicerExecutionModel_build)
find_package(SlicerExecutionModel REQUIRED)
include(${SlicerExecutionModel_USE_FILE})

set(ITK_DIR /work/boucaud/ITK/ITK_build)
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

### TEST project ###
set(TEST_SOURCES 
test_exec.cxx
)

SEMMacroBuildCLI(
NAME test_exec
TARGET_LIBRARIES ${ITK_LIBRARIES}
)

### files needed for test input and output verification
configure_file(
${CMAKE_CURRENT_SOURCE_DIR}/data/input_camera.png 
${CMAKE_CURRENT_BINARY_DIR}/data/input_camera.png
COPYONLY
)

configure_file(
${CMAKE_CURRENT_SOURCE_DIR}/data/output_camera.png 
${CMAKE_CURRENT_BINARY_DIR}/data/output_camera.png
COPYONLY
)

configure_file(
${CMAKE_CURRENT_SOURCE_DIR}/data/input_slice.png 
${CMAKE_CURRENT_BINARY_DIR}/data/input_slice.png
COPYONLY
)

configure_file(
${CMAKE_CURRENT_SOURCE_DIR}/data/output_slice.png 
${CMAKE_CURRENT_BINARY_DIR}/data/output_slice.png
COPYONLY
)

configure_file(
${CMAKE_CURRENT_SOURCE_DIR}/data/input_BrainT1Slice.png 
${CMAKE_CURRENT_BINARY_DIR}/data/input_BrainT1Slice.png
COPYONLY
)

configure_file(
${CMAKE_CURRENT_SOURCE_DIR}/data/output_BrainT1Slice.png 
${CMAKE_CURRENT_BINARY_DIR}/data/output_BrainT1Slice.png
COPYONLY
)


add_test(
NAME test_camera
COMMAND bin/test_exec ../test/data/input_camera.png ../test/data/output_camera.png
)

add_test(
NAME test_slice
COMMAND bin/test_exec ../test/data/input_slice.png ../test/data/output_slice.png
)
add_test(
NAME test_BrainT1Slice
COMMAND bin/test_exec ../test/data/input_BrainT1Slice.png ../test/data/output_BrainT1Slice.png
)
