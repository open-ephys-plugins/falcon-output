cmake_minimum_required(VERSION 3.11)
Project(Client)

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

include(FetchContent)
FetchContent_Declare(
    flatbuffers
    GIT_REPOSITORY "https://github.com/google/flatbuffers"
    GIT_TAG        v2.0.0
)
set(FLATBUFFERS_BUILD_FLATC ON CACHE INTERNAL "Enable the build of the flatbuffers compiler")
set(FLATBUFFERS_BUILD_TESTS OFF CACHE INTERNAL "Enable tests from flatbuffers lib.")
set(FLATBUFFERS_BUILD_FLATHASH OFF CACHE INTERNAL "Enable the build of flathash")
set(FLATBUFFERS_BUILD_CPP17 ON CACHE INTERNAL "Enable the build of c++17 test target")
set(FLATBUFFERS_INSTALL OFF CACHE INTERNAL "Enable the installation of targets.")
set(FLATBUFFERS_ENABLE_PCH OFF CACHE INTERNAL
       "Enable precompile headers support for 'flatbuffers' and 'flatc'. \"
        Only work if CMake support 'target_precompile_headers'. \"
        Only work if CMake supports 'target_precompile_headers'. \"
        This can speed up compilation time."
       )

FetchContent_MakeAvailable(flatbuffers)

include_directories(BEFORE SYSTEM "${flatbuffers_SOURCE_DIR}/include")

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/channel_generated.h
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../../Source/channel.fbs
    COMMAND flatc --cpp ${CMAKE_CURRENT_SOURCE_DIR}/../../Source/channel.fbs
)

add_custom_target(channelbuffer DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/channel_generated.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(Client client.cpp)

#copy the ZMQ dlls when installing
if (MSVC)
    set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/windows)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/windows/bin/${CMAKE_LIBRARY_ARCHITECTURE}/ DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/Release)
elseif(LINUX)
    set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/linux)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/linux/bin/${CMAKE_LIBRARY_ARCHITECTURE}/ DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/Release)
elseif(APPLE)
    set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/macos)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../libs/macos/bin/${CMAKE_LIBRARY_ARCHITECTURE}/ DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/Release)
endif()

#additional libraries, if needed
find_library(ZMQ_LIBRARIES NAMES libzmq-v142-mt-4_3_4 zmq zmq-v142-mt-4_3_4)
find_path(ZMQ_INCLUDE_DIRS zmq.h)

target_include_directories(Client PUBLIC ${ZMQ_INCLUDE_DIRS})
target_link_libraries(Client ${ZMQ_LIBRARIES})
target_compile_definitions(Client PRIVATE ZEROMQ $<$<PLATFORM_ID:Windows>:_SCL_SECURE_NO_WARNINGS>)

# target_link_libraries(Client cppzmq)
add_dependencies(Client channelbuffer)
