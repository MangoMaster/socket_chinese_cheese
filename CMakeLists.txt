cmake_minimum_required(VERSION 3.1.0)

project(Cheese)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
# Create code from resources files
set(CMAKE_AUTORCC on)

# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)
# Find the QtNetwork library
find_package(Qt5Network CONFIG REQUIRED)

include_directories(/usr/include/x86_64-linux-gnu/qt5)
include_directories(./src)

# Populate a CMake variable with the sources
set(MAIN_SRCS main.cpp src/mainwindow.qrc src/mainwindow.ui src/mainwindow.cpp src/modedialog.ui src/modedialog.cpp src/cheeseview.cpp src/cheesecontroller.cpp src/cheesescene.cpp src/cheesemodel.cpp src/cheesetcpconnection.cpp)
# Tell CMake to create the cheese executable
add_executable(cheese ${MAIN_SRCS})
# Use the Widgets & Network module from Qt 5
target_link_libraries(cheese Qt5::Widgets Qt5::Network)