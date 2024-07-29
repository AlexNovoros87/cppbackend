cmake_minimum_required(VERSION 3.11)
project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)


# ASYNC_SERVER-----------------------------------------------------------------------
set(server
  src/server/sdk.h src/server/cl_session_base.h src/server/cl_session.h src/server/cl_listener.h
  src/server/http_server.h src/server/rerror.cpp src/server/cl_session_base.cpp)

set(logger
  src/logger/logger.h src/logger/logger.cpp)

add_library(ASYNC ${server} ${logger})
target_link_libraries(ASYNC PUBLIC CONAN_PKG::boost)


# REQUEST HANDLER-----------------------------------------------------------------------
set(json
  src/json/json_loader.h
  src/json/json_loader.cpp)

set(server_helper
  src/req_helper/help.h src/req_helper/help.cpp src/req_helper/req_static_objects.h)

set(handler
  src/requester/api_handler.h src/requester/request_handler.cpp src/requester/request_handler.h)

set(launch
  src/parse_ticker/comparser.h src/parse_ticker/comparser.cpp src/parse_ticker/ticker.h src/parse_ticker/ticker.cpp)

set(srv_start ${json} ${handler} ${launch} ${server_helper})
add_library(REQEST_HANDLER ${srv_start})
target_link_libraries(REQEST_HANDLER PUBLIC CONAN_PKG::boost)
# ------------------------------------------------------------------


#COLISION -------------------------------------------------------
set(colision src/colis_detect/geom.h  src/colis_detect/collision_detector.h 
    src/colis_detect/collision_detector.cpp)
add_library(COLISION ${colision})

#--------------------------------------------------------------------------


# GAME LIBRARY-----------------------------------------------------
set(model
  src/model/infrastructure.h src/model/infrastructure.cpp src/model/tagged.h
  src/model/game_dog_session.h src/model/game_dog_session.cpp
)
set(game
  src/game/game.h src/game/game.cpp src/game/collizer.h src/game/collizer.cpp
  src/game/play_observer.cpp src/game/player.cpp)

  set(model_helper
  src/req_helper/declaration_structs.h src/req_helper/declaration_structs.cpp
  src/req_helper/model_help.h src/req_helper/model_help.cpp
)

set(tokenizer
  src/tokenizer/tokenizer.h src/tokenizer/tokenizer.cpp)

set(loot_gen
  src/loot_gen/loot_generator.h src/loot_gen/loot_generator.cpp
)

add_library(GM ${model} ${loot_gen} ${tokenizer} ${game} ${model_helper})
target_link_libraries(GM PUBLIC CONAN_PKG::boost COLISION)
# -------------------------------------------------------------------------


#SERIALISATOR--------------------------------------------------------------
set(serial src/serializer/serialize_overloads.h  src/serializer/serialize_overloads.cpp)
add_library(SERIALIZE ${serial})
target_link_libraries(SERIALIZE PUBLIC GM CONAN_PKG::boost)
# -------------------------------------------------------------------------


set (libs COLISION CONAN_PKG::boost GM SERIALIZE)

add_executable(game_server
  src/main.cpp
  src/sdk.h
)
target_link_libraries(game_server PUBLIC ${libs}  ASYNC REQEST_HANDLER)
 
add_executable(game_server_tests
 tests/loot_generator_tests.cpp
 )
 find_package(Threads REQUIRED)
 target_link_libraries(game_server_tests PUBLIC ${libs} CONAN_PKG::catch2 Threads::Threads)


 add_executable(game_server_serial
 tests/ser_tests.cpp
 src/json/json_loader.h
 src/json/json_loader.cpp
 )
 find_package(Threads REQUIRED)
 target_link_libraries(game_server_serial PUBLIC ${libs} CONAN_PKG::catch2 Threads::Threads)








 include(CTest)
include(${CONAN_BUILD_DIRS_CATCH2}/Catch.cmake)
catch_discover_tests(game_server_tests)
