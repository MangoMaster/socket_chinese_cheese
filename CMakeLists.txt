cmake_minimum_required(VERSION 3.1.0)

project(Cheese)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)

include_directories(/usr/include/x86_64-linux-gnu/qt5)
include_directories(./src)

# Populate a CMake variable with the sources
set(MAIN_SRCS main.cpp src/mainwindow.cpp src/mainwindow.ui src/cheeseview.cpp src/cheesecontroller.cpp src/cheesescene.cpp src/cheesemodel.cpp)
# Tell CMake to create the cheese executable
add_executable(cheese ${MAIN_SRCS})
# Use the Widgets module from Qt 5
target_link_libraries(cheese Qt5::Widgets)