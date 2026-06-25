cmake_minimum_required(VERSION 3.16)

project(CommTool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Quick QuickControls2 Graphs SerialPort)

qt_standard_project_setup()

add_library(CommModelLib STATIC
    CommModelLib/SerialPort/SerialService.cpp
    CommModelLib/Common/BaseCommService.cpp
)

target_include_directories(CommModelLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/CommModelLib
    ${CMAKE_CURRENT_SOURCE_DIR}/CommModelLib/SerialPort
    ${CMAKE_CURRENT_SOURCE_DIR}/CommModelLib/Common
)

target_link_libraries(CommModelLib PRIVATE
    Qt6::Core
    Qt6::SerialPort
)

qt_add_executable(CommTool
    main.cpp
    ViewModel/CommViewModel.cpp
    ViewModel/DataPlotViewModel.cpp
    ViewModel/MainViewModel.cpp
    Help/CommImpl.cpp
    Help/DataParser.cpp
    Help/crc.cpp
)

qt_add_qml_module(CommTool
    URI CommTool
    VERSION 1.0
    QML_FILES
        View/MainView.qml
        View/SideBarView.qml
        View/DataShowView.qml
        View/DataPlotView.qml
        View/SettingView.qml
        View/HelpView.qml
        View/AppSettingsDialog.qml
        View/CommonToolView/SerialSettingView.qml
        View/Component/LineChartWidget.qml
        View/Component/Button.qml
        View/Component/ChartStatusOverlay.qml
        View/Theme/ColorTheme.qml
        View/Theme/Toast.qml
        View/Theme/qmldir
    SOURCES
        ViewModel/CommViewModel.h
        ViewModel/CommViewModel.cpp
        ViewModel/DataPlotViewModel.h
        ViewModel/DataPlotViewModel.cpp
        ViewModel/MainViewModel.h
        ViewModel/MainViewModel.cpp
        ViewModel/AppSettingsViewModel.h
        ViewModel/AppSettingsViewModel.cpp
        Help/CommImpl.h
        Help/CommImpl.cpp
        Help/DataParser.h
        Help/DataParser.cpp
        Help/crc.h
        Help/crc.cpp
)

target_include_directories(CommTool PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/ViewModel
    ${CMAKE_CURRENT_SOURCE_DIR}/Help
    ${CMAKE_CURRENT_SOURCE_DIR}/CommModelLib
)

target_link_libraries(CommTool PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::Graphs
    Qt6::SerialPort
    CommModelLib
)

install(TARGETS CommTool
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)