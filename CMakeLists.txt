project (pineapple-pictures)

cmake_minimum_required (VERSION 3.9.5)

include (GNUInstallDirs)

set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTORCC ON)
set (QT_MINIMUM_VERSION "5.7.1")

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

# Install settings
if (UNIX)
    if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(CMAKE_INSTALL_PREFIX /usr)
    endif ()

    install (
        TARGETS ${EXE_NAME}
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )

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
endif (UNIX)

