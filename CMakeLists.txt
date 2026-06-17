cmake_minimum_required(VERSION 3.16)

project(app_serial_tool VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_QML_GENERATE_QMLLS_INI ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(app_serial_tool
    main.cpp
    src/core/serial_manager.cpp
    src/platform/platform_serial_factory.cpp
    src/platform/desktop/desktop_serial_port.cpp
)

# Android-specific sources
if(ANDROID)
    target_sources(app_serial_tool PRIVATE
        src/platform/android/android_serial_port.cpp
    )
endif()

qt_add_qml_module(app_serial_tool
    URI app_serial_tool
    QML_FILES
        Main.qml
)

set_target_properties(app_serial_tool PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_include_directories(app_serial_tool PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(app_serial_tool
    PRIVATE Qt6::Quick
)

# Android-specific configuration
if(ANDROID)
    set_target_properties(app_serial_tool PROPERTIES
        QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
    )
endif()

install(TARGETS app_serial_tool
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
