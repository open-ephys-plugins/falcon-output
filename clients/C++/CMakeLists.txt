cmake_minimum_required(VERSION 3.11)
Project(Client)

set(CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "Build architecture for Mac OS X" FORCE)

if (NOT CMAKE_LIBRARY_ARCHITECTURE)
	if (CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(CMAKE_LIBRARY_ARCHITECTURE "x64")
	else()
		set(CMAKE_LIBRARY_ARCHITECTURE "x86")
	endif()
endif()

set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
    Client
	$<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_WARNINGS>
	$<$<CONFIG:Debug>:DEBUG=1>
	$<$<CONFIG:Debug>:_DEBUG=1>
	$<$<CONFIG:Release>:NDEBUG=1>
	)


set(CONFIGURATION_FOLDER $<$<CONFIG:Debug>:Debug>$<$<NOT:$<CONFIG:Debug>>:Release>)

if (MSVC)
    set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/windows)
    set(FLATC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/windows/bin/${CMAKE_LIBRARY_ARCHITECTURE})
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/windows/bin/${CMAKE_LIBRARY_ARCHITECTURE}/ DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/${CONFIGURATION_FOLDER}
            FILES_MATCHING PATTERN "*.dll")
elseif(LINUX)
    set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/linux)
    set(FLATC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/linux/bin)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/linux/bin/${CMAKE_LIBRARY_ARCHITECTURE}/ DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/${CONFIGURATION_FOLDER}
            FILES_MATCHING PATTERN "*.so.*")
elseif(APPLE)
    set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/macos)
    set(FLATC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/macos/bin)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/macos/bin/${CMAKE_LIBRARY_ARCHITECTURE}/ DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/${CONFIGURATION_FOLDER}
            FILES_MATCHING PATTERN "*.dylib")
endif()

add_executable(Client client.cpp)
target_compile_features(Client PRIVATE cxx_std_17)

target_include_directories(Client PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/include)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/channel_generated.h
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../../Source/channel.fbs
    COMMAND ${FLATC_DIR}/flatc --cpp ${CMAKE_CURRENT_SOURCE_DIR}/../../Source/channel.fbs
)

add_custom_target(channelbuffer DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/channel_generated.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_dependencies(Client channelbuffer)

#additional libraries, if needed
find_library(ZMQ_LIBRARIES NAMES libzmq-v142-mt-4_3_4 zmq zmq-v142-mt-4_3_4)
find_path(ZMQ_INCLUDE_DIRS zmq.h)

target_include_directories(Client PUBLIC ${ZMQ_INCLUDE_DIRS})
target_link_libraries(Client ${ZMQ_LIBRARIES})
target_compile_definitions(Client PRIVATE ZEROMQ $<$<PLATFORM_ID:Windows>:_SCL_SECURE_NO_WARNINGS>)
