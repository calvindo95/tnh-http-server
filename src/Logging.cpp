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
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/utility/value_ref.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>

#include <string>
#include <iomanip>
#include <fstream>

#include <Logging.h>

BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", Logging::severity_level)
BOOST_LOG_ATTRIBUTE_KEYWORD(tag_attr, "Tag", std::string)

Logging::Logging(){}

// init() only has to be run once, typically at the beginning of the program to set the sinks
void Logging::init(){
    // Setup the common formatter for all sinks
    boost::log::formatter fmt = boost::log::expressions::stream
        << boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S:%f")
        << " [" << severity << "]"
        << boost::log::expressions::if_(boost::log::expressions::has_attr(tag_attr))[
               boost::log::expressions::stream << "[" << tag_attr << "] "
            ]
        << boost::log::expressions::smessage;

    // Initialize sink object
    typedef boost::log::sinks::synchronous_sink< boost::log::sinks::text_ostream_backend > text_sink;
    boost::shared_ptr< text_sink > sink = boost::make_shared<text_sink>();

    // initialize stream to write to all.log
    std::stringstream ssq;
    ssq << GET_TNH_LOG().c_str() << "/queue.log";
    sink->locked_backend()->add_stream(
        boost::make_shared<std::ofstream>(ssq.str()));
    sink->set_formatter(fmt);
        sink->set_filter(severity >= trace && (         // filter attributes
        boost::log::expressions::has_attr(tag_attr) && tag_attr == "QUEUE"));
    boost::log::core::get()->add_sink(sink);
    sink->locked_backend()->auto_flush(true);       // sets autoflush; this needs to set to false in prod(true for testing)

    // initialize stream to write to gentrace.log
    std::stringstream ssg;
    ssg << GET_TNH_LOG().c_str() << "/gentrace.log";
    sink = boost::make_shared<text_sink>();
    sink->locked_backend()->add_stream(
        boost::make_shared<std::ofstream>(ssg.str()));
    sink->set_formatter(fmt);
    sink->set_filter(severity >= trace && (         // filter attributes
        boost::log::expressions::has_attr(tag_attr) && tag_attr == "GENTRACE"));
    boost::log::core::get()->add_sink(sink);
    sink->locked_backend()->auto_flush(true);       // sets autoflush; needs to be false in prod(true for testing)

    // initialize stream to write to event.log
    std::stringstream sse;
    sse << GET_TNH_LOG().c_str() << "/event.log";
    sink = boost::make_shared<text_sink>();
    sink->locked_backend()->add_stream(
        boost::make_shared<std::ofstream>(sse.str()));
    sink->set_formatter(fmt);
    sink->set_filter(severity >= trace && (         // filter attributes
        boost::log::expressions::has_attr(tag_attr) && tag_attr == "EVENT"));
    boost::log::core::get()->add_sink(sink);
    sink->locked_backend()->auto_flush(true);       // sets autoflush; needs to be false in prod(true for testing)

    // add attributes
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