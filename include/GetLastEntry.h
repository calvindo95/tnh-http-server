#ifndef GETLASTENTRY_H
#define GETLASTENTRY_H

#include <httpserver.hpp>
#include <HTTPResources.h>

class get_last_entry : public httpserver::http_resource, public HTTPResources {
    private:
        DBQ m_dbq;

    public:
        std::shared_ptr<httpserver::http_response> render(const httpserver::http_request& req);
};

#endif