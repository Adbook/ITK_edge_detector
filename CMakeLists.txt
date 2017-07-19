project(main)

#enable_testing()

#set (CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS -std=c++0x)

set(SlicerExecutionModel_DIR /work/boucaud/SEM/SlicerExecutionModel_build)
find_package(SlicerExecutionModel REQUIRED)
include(${SlicerExecutionModel_USE_FILE})

set(ITK_DIR /work/boucaud/ITK/ITK_build)
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})


### SEM project ###
set(CLI_SOURCES 
main_cli.cxx
)

SEMMacroBuildCLI(
NAME main_cli
TARGET_LIBRARIES ${ITK_LIBRARIES}
)

add_executable(cli_exec ${CLI_SOURCES})
target_link_libraries(cli_exec ${ITK_LIBRARIES})

### Qt project ###

set(QT_DIR /work/boucaud/Qt/Qt_4.8.6_build)
find_package( Qt4 REQUIRED )
include( ${QT_USE_FILE} )
add_definitions( ${QT_DEFINITIONS} )
include_directories( ${CMAKE_BINARY_DIR} )



set(GUI_SOURCES 
main_gui.cxx
mainwindow.cxx
)

set ( MOC_HEADERS
mainwindow.hxx
)

set ( UIS
mainwindow.ui
)

set ( RESOURCES
# main.qrc
)

# using Qt meta-system (precompiler)
QT4_ADD_RESOURCES( RES_SOURCES ${RESOURCES} )
QT4_WRAP_UI( UI_HEADERS ${UIS} )
QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )

add_executable(gui_exec ${GUI_SOURCES} ${MOC_SRCS} ${RES_SOURCES} ${UI_HEADERS})
target_link_libraries(gui_exec ${ITK_LIBRARIES} ${QT_LIBRARIES})