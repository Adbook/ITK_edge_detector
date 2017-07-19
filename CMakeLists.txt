project(main)

#enable_testing()

set (CMAKE_CXX_STANDARD 11)

set(SlicerExecutionModel_DIR /work/boucaud/SEM/SlicerExecutionModel_build)
find_package(SlicerExecutionModel REQUIRED)
include(${SlicerExecutionModel_USE_FILE})

set(ITK_DIR /work/boucaud/ITK/ITK_build)
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

set(CLI_SOURCES 
main_cli.cxx
)

SEMMacroBuildCLI(
NAME main_cli
TARGET_LIBRARIES ${ITK_LIBRARIES}
)

add_executable(cli_exec ${CLI_SOURCES})

target_link_libraries(cli_exec ${ITK_LIBRARIES})
