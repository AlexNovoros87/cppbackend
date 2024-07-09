#include "cl_session_base.h"

namespace http_server {
void ReportError(beast::error_code ec, std::string_view what) {
    std::cerr << what << ": "sv << ec.message() << std::endl;
  
    #ifdef CONSOLE_LOGGING
         logger::LogErr(ec, what);
         #endif
   
}
}