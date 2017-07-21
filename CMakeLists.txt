project(main)

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

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


### CLI project ###
set(CLI_SOURCES 
main_cli.cxx
)

SEMMacroBuildCLI(
NAME main_cli
TARGET_LIBRARIES ${ITK_LIBRARIES}
)


### GUI project ###
set(VTK_DIR /work/boucaud/VTK/vtk_build)
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})


set(QT_USE_QTDESIGNER TRUE)
find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})


#add_definitions( ${QT_DEFINITIONS} )

include_directories( ${CMAKE_BINARY_DIR} )

#MARK_AS_ADVANCED(QT_QMAKE_EXECUTABLE)



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
#QT4_ADD_RESOURCES( RES_SOURCES ${RESOURCES} )
QT4_WRAP_UI( UI_HEADERS ${UIS} )
QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS} )

add_executable(gui_exec ${GUI_SOURCES} ${MOC_SRCS} ${RES_SOURCES} ${UI_HEADERS})
#target_link_libraries(gui_exec ${ITK_LIBRARIES} ${QT_LIBRARIES} ${VTK_LIBRARIES} QVTK)

  if(VTK_LIBRARIES)
    if(${VTK_VERSION} VERSION_LESS "6")
      target_link_libraries(gui_exec ${ITK_LIBRARIES} ${VTK_LIBRARIES} QVTK)
    else()
      target_link_libraries(gui_exec ${ITK_LIBRARIES} ${VTK_LIBRARIES})
    endif()
  else()
    target_link_libraries(gui_exec vtkHybrid QVTK vtkViews ${QT_LIBRARIES} ${ITK_LIBRARIES})
  endif()
endif()