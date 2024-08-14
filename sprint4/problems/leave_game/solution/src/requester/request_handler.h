#pragma once
#include "api_handler.h"
#include <mutex>

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

      mtx_.lock();
      auto parsed_target = ParseTarget(std::string(req.target()));
      request_handler::TypeRequest type_request = request_handler::GetTypeRequset(parsed_target);
      mtx_.unlock();

      try
      {

        if (type_request == request_handler::TypeRequest::API)
        {

          auto handle = [self = shared_from_this(), send,
                         req = std::forward<decltype(req)>(req), p_target = std::move(parsed_target)]
          {
            try
            {

              // Этот assert не выстрелит, так как лямбда-функция будет выполняться внутри strand
              assert(self->api_strand_.running_in_this_thread());
              return send(APIHandler(std::move(std::move(req)), self->game_, std::move(p_target)).MakeResponce());
            }
            catch (const std::exception &ex)
            {
              std::string mist = "*****IN API*****";
              mist.append(std::string(ex.what()));
              send(self->ReportServerError(req.version(), req.keep_alive(), mist));
            }
          };
          return net::dispatch(api_strand_, std::move(handle));
        }
        else if (type_request == request_handler::TypeRequest::STATIC)
        {
          try
          {
            send(MakeStaticRespone(req.version(), req.keep_alive(), std::string(req.target())));
          }
          catch (const std::exception &ex)
          {
            std::string mist = "*****IN STATIC*****" + std::string(ex.what());
            send(ReportServerError(req.version(), req.keep_alive(), std::move(mist)));
          }
        }
      }
      catch (...)
      {
        send(ReportServerError(req.version(), req.keep_alive(), "ABNORMAL AND UNKNOWN EXC"));
      }
    }

  private:
    VariantResponse ReportServerError(unsigned version, bool keep_alive, std::string error_what) const;
    VariantResponse MakeStaticRespone(unsigned version, bool keep_alive, std::string target);
    std::mutex mtx_;
  };

} // namespace http_handler