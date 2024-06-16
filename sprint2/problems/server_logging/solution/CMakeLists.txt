cmake_minimum_required(VERSION 3.11)
project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(game_server
	src/main.cpp
	src/sdk.h
	src/model.h
	src/model.cpp
	src/tagged.h
	src/json_loader.h
	src/json_loader.cpp
	src/request_handler.cpp
	src/request_handler.h
	src/req_static_objects.h
   
	src/server/sdk.h
	src/server/cl_session_base.h
	src/server/cl_session.h
	src/server/cl_listener.h
	src/server/http_server.h
	src/server/rerror.cpp
	src/server/cl_session_base.cpp

    src/logger/logger.h
	src/logger/logger.cpp
)
#target_include_directories(game_server PRIVATE CONAN_PKG::boost)
target_link_libraries(game_server PRIVATE CONAN_PKG::boost Threads::Threads)

