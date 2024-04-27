# SPDX-FileCopyrightText: 2022 - 2024 Gary Wang <git@blumia.net>
#
# SPDX-License-Identifier: MIT

cmake_minimum_required (VERSION 3.9.5)

project (pineapple-pictures)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)

include (GNUInstallDirs)
include (FeatureSummary)

option (EXIV2_METADATA_SUPPORT "Better image metadata support via libexiv2" ON)
option (PREFER_QT_5 "Prefer to use Qt 5" OFF)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTORCC ON)

if (PREFER_QT_5)
    find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core)
else ()
    find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
endif ()

if (${QT_VERSION_MAJOR} EQUAL "5")
    set (QT_MINIMUM_VERSION "5.15.2")
else ()
    set (QT_MINIMUM_VERSION "6.4")
endif ()

find_package(Qt${QT_VERSION_MAJOR} ${QT_MINIMUM_VERSION} REQUIRED
    COMPONENTS Widgets Svg LinguistTools
    OPTIONAL_COMPONENTS DBus
)

if (${QT_VERSION_MAJOR} EQUAL "6")
    find_package(Qt${QT_DEFAULT_MAJOR_VERSION} ${QT_MINIMUM_VERSION} CONFIG REQUIRED SvgWidgets)
endif ()

if (EXIV2_METADATA_SUPPORT)
    find_package(LibExiv2)
    set_package_properties(LibExiv2 PROPERTIES
        URL "https://www.exiv2.org"
        DESCRIPTION "image metadata support"
        TYPE OPTIONAL
        PURPOSE "Bring better image metadata support"
    )
endif ()

#LibExiv2_FOUND
set (PPIC_CPP_FILES
    app/main.cpp
    app/framelesswindow.cpp
    app/mainwindow.cpp
    app/actionmanager.cpp
    app/graphicsview.cpp
    app/graphicsscene.cpp
    app/bottombuttongroup.cpp
    app/navigatorview.cpp
    app/opacityhelper.cpp
    app/toolbutton.cpp
    app/settings.cpp
    app/settingsdialog.cpp
    app/aboutdialog.cpp
    app/metadatamodel.cpp
    app/metadatadialog.cpp
    app/exiv2wrapper.cpp
    app/playlistmanager.cpp
)

set (PPIC_HEADER_FILES
    app/framelesswindow.h
    app/mainwindow.h
    app/actionmanager.h
    app/graphicsview.h
    app/graphicsscene.h
    app/bottombuttongroup.h
    app/navigatorview.h
    app/opacityhelper.h
    app/toolbutton.h
    app/settings.h
    app/settingsdialog.h
    app/aboutdialog.h
    app/metadatamodel.h
    app/metadatadialog.h
    app/exiv2wrapper.h
    app/playlistmanager.h
)

set (PPIC_QRC_FILES
    assets/resources.qrc
)

set (PPIC_RC_FILES
    # yeah, it's empty.
)

set (EXE_NAME ppic)

# Translation
file (GLOB PPIC_TS_FILES app/translations/*.ts)
set (PPIC_CPP_FILES_FOR_I18N ${PPIC_CPP_FILES})

if (WIN32)
    list(APPEND PPIC_RC_FILES assets/pineapple-pictures.rc)
endif ()

add_executable (${EXE_NAME} WIN32
    ${PPIC_HEADER_FILES}
    ${PPIC_CPP_FILES}
    ${PPIC_QRC_FILES}
    ${PPIC_RC_FILES}
)

qt_create_translation(PPIC_QM_FILES ${PPIC_CPP_FILES_FOR_I18N} ${PPIC_TS_FILES})

target_sources(${EXE_NAME} PRIVATE ${PPIC_QM_FILES})

target_link_libraries (${EXE_NAME} Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Svg)
if (${QT_VERSION_MAJOR} EQUAL "6")
    target_link_libraries (${EXE_NAME} Qt::SvgWidgets)
endif ()

if (LibExiv2_FOUND)
    message(INFO ${LibExiv2_INCLUDE_DIRS})
    target_include_directories(${EXE_NAME}
    PRIVATE
        ${LibExiv2_INCLUDE_DIRS}
    )
    target_link_libraries (${EXE_NAME}
        LibExiv2::LibExiv2
    )
    target_compile_definitions(${EXE_NAME} PRIVATE
        HAVE_EXIV2_VERSION="${LibExiv2_VERSION}"
    )
endif ()

if (TARGET Qt5::DBus OR TARGET Qt6::DBus)
    target_link_libraries (${EXE_NAME}
        Qt${QT_VERSION_MAJOR}::DBus
    )
    target_compile_definitions(${EXE_NAME} PRIVATE
        HAVE_QTDBUS
    )
endif()

# Extra build settings
if (WIN32)
    target_compile_definitions(${EXE_NAME} PRIVATE
        FLAG_PORTABLE_MODE_SUPPORT=1
    )
endif ()

# Helper macros for parsing and setting project version from `git describe --long` result
macro (ppic_set_version_via_describe _describe_long)
    string (
        REGEX REPLACE
        "^([0-9a-z.]*)-[0-9]+-g[0-9a-f]*$"
        "\\1"
        _tag_parts
        "${_describe_long}"
    )
    list (GET _tag_parts 0 _matched_tag_version)
    if ("${_matched_tag_version}" MATCHES "^[0-9]+\\.[0-9]+\\.[0-9]+$")
        string (
            REGEX REPLACE
            "^([0-9]+)\\.([0-9]+)\\.([0-9]+).*$"
            "\\1;\\2;\\3"
            _ver_parts
            "${_matched_tag_version}"
        )
        list (GET _ver_parts 0 CPACK_PACKAGE_VERSION_MAJOR)
        list (GET _ver_parts 1 CPACK_PACKAGE_VERSION_MINOR)
        list (GET _ver_parts 2 CPACK_PACKAGE_VERSION_PATCH)
    endif ()
endmacro ()

# Version setup
if (EXISTS "${CMAKE_SOURCE_DIR}/.git")
    find_package(Git)
    set_package_properties(Git PROPERTIES TYPE OPTIONAL PURPOSE "Determine exact build version.")
    if (GIT_FOUND)
        execute_process (
            COMMAND ${GIT_EXECUTABLE} describe --tags --always --long
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            OUTPUT_VARIABLE _git_describe_long
        )
        string (REGEX REPLACE "\n" "" _git_describe_long "${_git_describe_long}")
        ppic_set_version_via_describe(${_git_describe_long})
        target_compile_definitions(${EXE_NAME} PRIVATE
            GIT_DESCRIBE_VERSION_STRING="${_git_describe_long}"
        )
    endif ()
endif ()

# Install settings
if (WIN32)
    # TODO: try to avoid install to a "bin" subfolder under windows...
    #       when fixed, don't forget to update the CI config file...
elseif (UNIX)
    if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(CMAKE_INSTALL_PREFIX /usr)
    endif ()

    # install icon
    install (
        FILES assets/icons/app-icon.svg
        DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps"
        RENAME net.blumia.pineapple-pictures.svg
    )

    # install shortcut
    install (
        FILES dist/net.blumia.pineapple-pictures.desktop
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications"
    )

    # install app metadata file for appstream (and some other stuff using this metadata like snapcraft)
    install (
        FILES dist/appstream/net.blumia.pineapple-pictures.metainfo.xml
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/metainfo"
    )
endif()

set (INSTALL_TARGETS_DEFAULT_ARGS
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Devel
)

install (
    TARGETS ${EXE_NAME}
    ${INSTALL_TARGETS_DEFAULT_ARGS}
)

if (WIN32)
    set (QM_FILE_INSTALL_DIR "${CMAKE_INSTALL_BINDIR}/translations")
else ()
    set (QM_FILE_INSTALL_DIR "${CMAKE_INSTALL_FULL_DATADIR}/pineapple-pictures/translations")
    target_compile_definitions(${EXE_NAME}
        PRIVATE QM_FILE_INSTALL_DIR=${QM_FILE_INSTALL_DIR}
    )
endif ()

install (
    FILES ${PPIC_QM_FILES}
    DESTINATION ${QM_FILE_INSTALL_DIR}
)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

# CPACK: General Settings
set (CPACK_GENERATOR "TBZ2")
set (CPACK_PACKAGE_NAME "pineapple-pictures")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Yet another image viewer")
set (CPACK_PACKAGE_VENDOR "Gary Wang")
set (CPACK_PACKAGE_CONTACT "https://github.com/BLumia/pineapple-pictures/issues/")
if (WIN32)
    # ...
elseif (APPLE)
    # ...
elseif (UNIX)
    set (CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
    set (CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    set (CPACK_DEBIAN_PACKAGE_RECOMMENDS "kimageformat-plugins")
endif()

include(CPack)
