# Bundled libraries for Windows
add_library(websockets SHARED IMPORTED GLOBAL)
add_library(curl SHARED IMPORTED GLOBAL)

set_target_properties(websockets PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}\\prebuilt\\websockets.dll")
set_target_properties(websockets PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}\\prebuilt\\websockets.lib")
set_target_properties(websockets PROPERTIES  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}\\include")

set_target_properties(curl PROPERTIES IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}\\prebuilt\\libcurl.dll")
set_target_properties(curl PROPERTIES IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}\\prebuilt\\libcurl.lib")
set_target_properties(curl PROPERTIES  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}\\include")