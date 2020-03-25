cmake_minimum_required(VERSION 3.15)
project(learning_dbus)

set(CMAKE_CXX_STANDARD 20)

add_executable(learning_dbus main.cpp)

find_package(PkgConfig)

pkg_check_modules(DBUS REQUIRED dbus-1)
include_directories(${DBUS_INCLUDE_DIRS})
link_directories(${DBUS_LIBRARY_DIRS})


target_link_libraries(learning_dbus ${DBUS_LIBRARIES})