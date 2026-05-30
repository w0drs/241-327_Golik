cmake_minimum_required(VERSION 3.16)

project(lab3 VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Network)

set(GENERATED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/generated/client)

set(GENERATED_SOURCES
    ${GENERATED_DIR}/OAIAnime.cpp
    ${GENERATED_DIR}/OAIAnimeCreate.cpp
    ${GENERATED_DIR}/OAIAnimeListResponse.cpp
    ${GENERATED_DIR}/OAIAnimeUpdate.cpp
    ${GENERATED_DIR}/OAIDefaultApi.cpp
    ${GENERATED_DIR}/OAIHelpers.cpp
    ${GENERATED_DIR}/OAIHttpFileElement.cpp
    ${GENERATED_DIR}/OAIHttpRequest.cpp
    ${GENERATED_DIR}/OAIHTTPValidationError.cpp
    ${GENERATED_DIR}/OAIOauth.cpp
    ${GENERATED_DIR}/OAIValidationError.cpp
    ${GENERATED_DIR}/OAIValidationError_loc_inner.cpp
)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    ${GENERATED_SOURCES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(lab3
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(lab3
        ${PROJECT_SOURCES}
    )
endif()

target_include_directories(lab3 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/client
)

target_link_libraries(lab3 PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.lab3)
endif()
set_target_properties(lab3 PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS lab3
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(lab3)
endif()
