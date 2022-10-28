cmake_minimum_required(VERSION 3.10)

add_library(minizip STATIC IMPORTED GLOBAL)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
add_library(zlib SHARED IMPORTED GLOBAL)
set_target_properties(minizip PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}\\libs\\minizip.lib")
set_target_properties(zlib PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}\\libs\\zlibwapi.dll")
set_target_properties(zlib PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}\\libs\\zlibwapi.lib")
else()
set_target_properties(minizip PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/headers/linux" IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/libs/minizip.so")
endif()