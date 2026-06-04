#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/attributes.hpp>
#include <boost/log/sources/basic_logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/utility/value_ref.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>

#include <string>
#include <iomanip>
#include <filesystem>

#include <Logging.h>

BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", Logging::severity_level)
BOOST_LOG_ATTRIBUTE_KEYWORD(tag_attr, "Tag", std::string)

static void add_sink(const boost::log::formatter& fmt, const std::string& log_dir, const std::string& name, const std::string& tag){
    typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend> file_sink;

    auto backend = boost::make_shared<boost::log::sinks::text_file_backend>(
        boost::log::keywords::file_name = log_dir + "/" + name + "_0.log",
        boost::log::keywords::rotation_size = 5 * 1024 * 1024,
        boost::log::keywords::open_mode = std::ios::app
    );

    // Before Boost opens a new _0, shift existing files up: _8→_9, …, _0→_1.
    // Renaming an open file is safe on Linux; the fd remains valid until Boost closes it.
    backend->set_close_handler([log_dir, name](boost::log::sinks::text_file_backend::stream_type&) {
        namespace fs = std::filesystem;
        for (int i = 8; i >= 0; --i) {
            fs::path src = log_dir + "/" + name + "_" + std::to_string(i) + ".log";
            fs::path dst = log_dir + "/" + name + "_" + std::to_string(i + 1) + ".log";
            if (fs::exists(src))
                fs::rename(src, dst);
        }
    });

    backend->auto_flush(true);

    auto sink = boost::make_shared<file_sink>(backend);
    sink->set_formatter(fmt);
    sink->set_filter(severity >= Logging::trace && (
        boost::log::expressions::has_attr(tag_attr) && tag_attr == tag));
    boost::log::core::get()->add_sink(sink);
}

Logging::Logging(){}

// init() only has to be run once, typically at the beginning of the program to set the sinks
void Logging::init(){
    boost::log::formatter fmt = boost::log::expressions::stream
        << boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S:%f")
        << " [" << severity << "]"
        << boost::log::expressions::if_(boost::log::expressions::has_attr(tag_attr))[
               boost::log::expressions::stream << "[" << tag_attr << "] "
            ]
        << boost::log::expressions::smessage;

    const std::string log_dir = GET_TNH_LOG();

    add_sink(fmt, log_dir, "queue", "QUEUE");
    add_sink(fmt, log_dir, "gentrace", "GENTRACE");
    add_sink(fmt, log_dir, "event", "EVENT");

    boost::log::add_common_attributes();
}

void Logging::log_trace(const std::string& msg, const std::string& filter){
    boost::log::sources::severity_logger_mt<severity_level> severity_log;       // need to use _mt to allow multithreading
    char* buffer = new char[filter.length() +1];
    std::strcpy(buffer, filter.c_str());
    
    BOOST_LOG_SCOPED_THREAD_TAG("Tag", buffer);
    BOOST_LOG_SEV(severity_log, Logging::severity_level::trace) << msg;
}

void Logging::log_trace(const std::stringstream& msg, const std::string& filter){
    boost::log::sources::severity_logger_mt<severity_level> severity_log;       // need to use _mt to allow multithreading
    char* buffer = new char[filter.length() +1];
    std::strcpy(buffer, filter.c_str());
    
    BOOST_LOG_SCOPED_THREAD_TAG("Tag", buffer);
    BOOST_LOG_SEV(severity_log, Logging::severity_level::trace) << msg.str();
}

void Logging::log(severity_level sl, const std::stringstream& msg, const std::string& filter){
    boost::log::sources::severity_logger_mt<severity_level> severity_log;       // need to use _mt to allow multithreading
    char* buffer = new char[filter.length() +1];
    std::strcpy(buffer, filter.c_str());
    
    BOOST_LOG_SCOPED_THREAD_TAG("Tag", buffer);
    BOOST_LOG_SEV(severity_log, sl) << msg.str();
}

void Logging::log(severity_level sl, const std::string& msg, const std::string& filter){
    boost::log::sources::severity_logger_mt<severity_level> severity_log;       // need to use _mt to allow multithreading
    char* buffer = new char[filter.length() +1];
    std::strcpy(buffer, filter.c_str());
    
    BOOST_LOG_SCOPED_THREAD_TAG("Tag", buffer);
    BOOST_LOG_SEV(severity_log, sl) << msg;
}

// helper function to make human-readable severity level
std::ostream& operator<<(std::ostream& strm, Logging::severity_level level)
{
    static const char* strings[] =
    {
        "TRACE",
        "INFO",
        "NORMAL",
        "NOTIFICATION",
        "WARNING",
        "ERROR",
        "CRITICAL"
    };

    if (static_cast< std::size_t >(level) < sizeof(strings) / sizeof(*strings))
        strm << strings[level];
    else
        strm << static_cast< int >(level);

    return strm;
}

std::string Logging::GET_TNH_LOG(){
    char* buffer = getenv("TNH_LOG");
    if(buffer != NULL){
        std::string option = static_cast<std::string>(buffer);

        return option;
    }
    return NULL;
}