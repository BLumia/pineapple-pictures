project (pineapple-pictures)

cmake_minimum_required (VERSION 3.9.5)

include (GNUInstallDirs)

set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTORCC ON)
set (QT_MINIMUM_VERSION "5.10")

find_package(Qt5 ${QT_MINIMUM_VERSION} CONFIG REQUIRED Widgets Svg LinguistTools)

set (PPIC_CPP_FILES
    main.cpp
    mainwindow.cpp
    graphicsview.cpp
    graphicsscene.cpp
    bottombuttongroup.cpp
    navigatorview.cpp
    opacityhelper.cpp
    toolbutton.cpp
)

set (PPIC_HEADER_FILES
    mainwindow.h
    graphicsview.h
    graphicsscene.h
    bottombuttongroup.h
    navigatorview.h
    opacityhelper.h
    toolbutton.h
)

set (PPIC_QRC_FILES
    resources.qrc
)

set (PPIC_RC_FILES
    # yeah, it's empty.
)

set (EXE_NAME ppic)

# Translation
file (GLOB PPIC_TS_FILES languages/*.ts)
set (PPIC_CPP_FILES_FOR_I18N ${PPIC_CPP_FILES})

qt5_create_translation(PPIC_QM_FILES ${PPIC_CPP_FILES_FOR_I18N} ${PPIC_TS_FILES})

if (WIN32)
    list(APPEND PPIC_RC_FILES pineapple-pictures.rc)
endif ()

add_executable (${EXE_NAME}
    ${PPIC_HEADER_FILES}
    ${PPIC_CPP_FILES}
    ${PPIC_QRC_FILES}
    ${PPIC_RC_FILES}
    ${PPIC_QM_FILES}
)

target_link_libraries (${EXE_NAME} Qt5::Widgets Qt5::Svg)

# Extra build settings
if (WIN32)
    set_property (
        TARGET ${EXE_NAME}
        PROPERTY WIN32_EXECUTABLE true
    )
endif ()

# Helper macros for install settings
macro (ppic_convert_to_relative_path _var)
    # Make sure _var is a relative path
    if (IS_ABSOLUTE "${${_var}}")
        file (RELATIVE_PATH ${_var} "${CMAKE_INSTALL_PREFIX}" "${${_var}}")
    endif ()
endmacro ()

# Install settings
if (WIN32)
    # FIXME: try to avoid install to a "bin" subfolder under windows...
    #        when fixed, don't forget to update the CI config file...
    set (BIN_INSTALL_DIR "") # seems useless, don't know why...
elseif (UNIX)
    if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(CMAKE_INSTALL_PREFIX /usr)
    endif ()

    set (BIN_INSTALL_DIR "${CMAKE_INSTALL_BINDIR}") # relative, usually "bin"
    ppic_convert_to_relative_path(BIN_INSTALL_DIR)
    set (LIB_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}") # "lib" or "lib64"
    ppic_convert_to_relative_path(LIB_INSTALL_DIR)

    # install icon
    install (
        FILES icons/app-icon.svg
        DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/48x48/apps"
        RENAME pineapple-pictures.svg
    )

    # install shortcut
    install (
        FILES pineapple-pictures.desktop
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/applications"
    )
endif()

set (INSTALL_TARGETS_DEFAULT_ARGS
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}
    LIBRARY DESTINATION ${LIB_INSTALL_DIR}
    ARCHIVE DESTINATION ${LIB_INSTALL_DIR} COMPONENT Devel
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

# CPACK: General Settings
set (CPACK_GENERATOR "TBZ2")
set (CPACK_PACKAGE_NAME "pineapple-pictures")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "Yet another image viewer")
set (CPACK_PACKAGE_VENDOR "Gary Wang")
set (CPACK_PACKAGE_CONTACT "https://github.com/BLumia/PineapplePictures/issues/")
if (WIN32)
    # ...
elseif (APPLE)
    # ...
elseif (UNIX)
    set (CPACK_SYSTEM_NAME "${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
    set (CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5svg5")
    set (CPACK_DEBIAN_PACKAGE_RECOMMENDS "kimageformat-plugins")
endif()

include(CPack)
