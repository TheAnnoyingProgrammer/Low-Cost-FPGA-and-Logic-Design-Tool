cmake_minimum_required(VERSION 3.16)

project(BuddyBoard_v1 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    01_FrontEnd/main.cpp
    01_FrontEnd/00_MainWindow/mainwindow.cpp
    01_FrontEnd/00_MainWindow/mainwindow.h
    01_FrontEnd/01_MenuBar/00_MenuBar.cpp
    01_FrontEnd/01_MenuBar/00_MenuBar.h
    01_FrontEnd/01_MenuBar/01_File.cpp
    01_FrontEnd/01_MenuBar/02_Edit.cpp
    01_FrontEnd/01_MenuBar/03_Tools.cpp
    01_FrontEnd/01_MenuBar/04_Help.cpp
    02_BackEnd/00_ClassStructures/00_Project.cpp
    02_BackEnd/00_ClassStructures/00_Project.h
    02_BackEnd/00_ClassStructures/01-S_LogicBlockSupport.h
    02_BackEnd/00_ClassStructures/01-0_LogicBlockDefinition.h
    02_BackEnd/00_ClassStructures/01-0_LogicBlockDefinition.cpp
    02_BackEnd/00_ClassStructures/01-1_LogicBlockInstance.h
    02_BackEnd/00_ClassStructures/01-1_LogicBlockInstance.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(BuddyBoard_v1
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(BuddyBoard_v1 SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(BuddyBoard_v1
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(BuddyBoard_v1 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(BuddyBoard_v1 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.BuddyBoard_v1)
endif()

set_target_properties(BuddyBoard_v1 PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS BuddyBoard_v1
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(BuddyBoard_v1)
endif()
