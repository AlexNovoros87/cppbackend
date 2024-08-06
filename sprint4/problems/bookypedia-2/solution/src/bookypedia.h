#pragma once


#include "app/use_cases_impl.h"
#include "postgres/postgres.h"

namespace bookypedia {

struct AppConfig {
    std::string db_url;
};

class Application {
public:
    explicit Application(const AppConfig& config);

    void Run();

private:
    // postgres::Database db_; /* pqxx::connection connection_;
    //                            AuthorRepositoryImpl authors_{connection_};
                                             
    //                         */

    app::UseCasesImpl use_cases_;
};

}  // namespace bookypedia
