cmake_minimum_required(VERSION 3.16)

project(screen_translate LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 自动 moc/uic/rcc
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Qt
find_package(Qt6 COMPONENTS Core Gui Widgets Network REQUIRED)

# 可执行文件：把新的 cpp 加进来（同级目录）
add_executable(screen_translate
    src/main.cpp
    src/mainwindow.cpp
    src/screensnippet.cpp
    src/geminiapihandler.cpp
    src/settingsdialog.cpp
    src/framelessdialog.cpp
    src/titlebar.cpp
    res/resources.qrc
)

# （可选但推荐）把含 Q_OBJECT 的头文件也声明给目标，AUTOMOC 更稳
target_sources(screen_translate PRIVATE
    src/mainwindow.h
    src/screensnippet.h
    src/geminiapihandler.h
    src/settingsdialog.h
    src/framelessdialog.h
    src/titlebar.h
)

target_link_libraries(screen_translate PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
)
