add_subdirectory(minizip)

add_library (handle_json STATIC make_json.cpp)


add_library (handle_files STATIC file_management.cpp uuid_generator.cpp)
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
target_link_libraries(handle_files PUBLIC zlib)
else()
target_link_libraries(handle_files PUBLIC minizip)
endif()
add_library (generate_libs STATIC obfuscator.cpp manifest.cpp item.cpp entity.cpp)
target_link_libraries(generate_libs PUBLIC handle_json)
target_link_libraries(generate_libs PUBLIC handle_files)