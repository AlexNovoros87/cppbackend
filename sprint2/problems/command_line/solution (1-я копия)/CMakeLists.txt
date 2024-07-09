cmake_minimum_required(VERSION 3.11)
project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)


include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set (model  
src/model/infrastructure.h
src/model/infrastructure.cpp
src/model/tagged.h
src/model/game_dog_session.h
src/model/game_dog_session.cpp 
)

set(server  
src/server/sdk.h
src/server/cl_session_base.h
src/server/cl_session.h
src/server/cl_listener.h
src/server/http_server.h
src/server/rerror.cpp
src/server/cl_session_base.cpp
)

set(logger 
src/logger/logger.h
src/logger/logger.cpp
)

set(json 
src/json/json_loader.h
src/json/json_loader.cpp
)


set (game  
src/game/game.h
src/game/game.cpp
)

set (helper 
src/req_helper/help.h
src/req_helper/help.cpp
src/req_helper/req_static_objects.h
src/req_helper/declaration_structs.h
src/req_helper/declaration_structs.cpp

src/req_helper/model_help.h
src/req_helper/model_help.cpp

)

set (handler
src/requester/api_handler.h
src/requester/request_handler.cpp
src/requester/request_handler.h
)

set (tokenizer
src/tokenizer/tokenizer.h
src/tokenizer/tokenizer.cpp
)


set (launch 
  src/parse_ticker/comparser.h
  src/parse_ticker/comparser.cpp
  src/parse_ticker/ticker.h
)

add_executable(game_server
	src/main.cpp
	src/sdk.h
	
    	
    ${model}
	${server}
	${logger}
	${json}
    ${game} 
	${helper}      	
    ${handler}
	${tokenizer}
	${launch}

	
)
target_link_libraries(game_server PRIVATE CONAN_PKG::boost)


