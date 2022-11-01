# Boilerplate
cmake_minimum_required(VERSION 3.10)
project(pilepoxyl)

# Packages, one by one
add_subdirectory(generate_libs)
add_subdirectory(pilepoxyl)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
add_subdirectory(winext)
else()
find_package(libwebsockets REQUIRED)
find_package(CURL REQUIRED)
endif()

# Executables
add_executable(runpp runpp.cpp)
target_link_libraries(runpp handle_json)
target_link_libraries(runpp handle_files)
target_link_libraries(runpp pilepoxyl)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
add_custom_command(TARGET runpp POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/winext/prebuilt/libcrypto-3-x64.dll" "${CMAKE_BINARY_DIR}/libcrypto-3-x64.dll")
add_custom_command(TARGET runpp POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/winext/prebuilt/libssl-3-x64.dll" "${CMAKE_BINARY_DIR}/libssl-3-x64.dll")
add_custom_command(TARGET runpp POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/winext/prebuilt/websockets.dll" "${CMAKE_BINARY_DIR}/websockets.dll")
add_custom_command(TARGET runpp POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/winext/prebuilt/libcurl.dll" "${CMAKE_BINARY_DIR}/libcurl.dll")
add_custom_command(TARGET runpp POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/generate_libs/minizip/libs/zlibwapi.dll" "${CMAKE_BINARY_DIR}/zlibwapi.dll")
endif()

add_executable (generate_tests tests/all_in_one_test.cpp)
target_link_libraries(generate_tests PUBLIC generate_libs)

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
add_custom_command(TARGET generate_tests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/generate_libs/minizip/libs/zlibwapi.dll" "${CMAKE_BINARY_DIR}/zlibwapi.dll")
endif()

add_custom_command(TARGET runpp POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/config
	${CMAKE_BINARY_DIR}/config)

add_custom_command(TARGET runpp POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/data
	${CMAKE_BINARY_DIR}/data)

add_custom_command(TARGET runpp POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/commands
	${CMAKE_BINARY_DIR}/commands)

add_custom_command(TARGET runpp POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory
	${CMAKE_BINARY_DIR}/tmp)

add_custom_command(TARGET runpp POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy
	${CMAKE_SOURCE_DIR}/FIRSTRUN
	${CMAKE_BINARY_DIR}/FIRSTRUN)

add_custom_command(TARGET generate_tests POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_SOURCE_DIR}/resources
	${CMAKE_BINARY_DIR}/resources)