#include <nlohmann/json.hpp>

#include <TSQueue.h>
#include <Logging.h>

class HTTPResources{
    protected:
        Logging m_logger;
        TSQueue<nlohmann::json> m_tsq;
};