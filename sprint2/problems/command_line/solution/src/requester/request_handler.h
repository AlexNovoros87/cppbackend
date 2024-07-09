#pragma once
#include"api_handler.h"
#include<mutex>

namespace request_handler
{
  class RequestHandler : public std::enable_shared_from_this<RequestHandler>
  {
    using Strand = net::strand<net::io_context::executor_type>;
    
    api::Play &game_;
    fs::path way_to_static;
    Strand api_strand_;
    const tcp::endpoint endpoint_;
    

  public:
    explicit RequestHandler(api::Play &game, fs::path path_to_static, Strand api_strand, tcp::endpoint endpoint)
        : game_{game}, way_to_static(fs::weakly_canonical(path_to_static)), api_strand_{api_strand}, endpoint_(endpoint)
    {
    }

    RequestHandler(const RequestHandler &) = delete;
    RequestHandler &operator=(const RequestHandler &) = delete;

    template <typename Body, typename Allocator, typename Send>
    void operator()(http::request<Body, http::basic_fields<Allocator>> &&req, Send &&send)
    {      
      auto parsed_target = ParseTarget(std::string(req.target()));
      request_handler::TypeRequest type_request = request_handler::GetTypeRequset(parsed_target);
      
      try
      {
        
        
        if (type_request == request_handler::TypeRequest::API)
        {
          
           auto handle = [self = shared_from_this(), send,
                                    req = std::forward<decltype(req)>(req), &parsed_target ] {
                         try {
                             // Этот assert не выстрелит, так как лямбда-функция будет выполняться внутри strand
                             assert(self->api_strand_.running_in_this_thread());
                             return send(APIHandler(std::move(std::move(req)), self->game_, parsed_target).MakeResponce());
                          } catch (...) {
                            
                             send(self->ReportServerError(req.version(), req.keep_alive()));
                         }
                     };
                     return net::dispatch(api_strand_, handle);
                             
        }
        else if (type_request == request_handler::TypeRequest::STATIC)
        {
          send(MakeStaticRespone(req.version(), req.keep_alive(), std::string(req.target())));
        }
      
      }
      catch (...)
      {
        send(ReportServerError(req.version(), req.keep_alive()));
      }
    }

  private:
    VariantResponse ReportServerError(unsigned version, bool keep_alive) const;

    VariantResponse MakeStaticRespone(unsigned version, bool keep_alive, std::string target);
  };

} // namespace http_handler