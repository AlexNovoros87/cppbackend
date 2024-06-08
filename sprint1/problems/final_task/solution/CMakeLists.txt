cmake_minimum_required(VERSION 3.11)

#gameserver ../../data/config.json


project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(Boost 1.78.0 REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

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
   
	src/server/sdk.h
	src/server/cl_session_base.h
	src/server/cl_session.h
	src/server/cl_listener.h
	src/server/http_server.h
	src/server/rerror.cpp
)
target_link_libraries(game_server PRIVATE Threads::Threads)
