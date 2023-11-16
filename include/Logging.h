#ifndef LOGGING_H
#define LOGGING_H

#include <string>
#include <ostream>
#include <fstream>

class Logging{
    private:
        bool auto_flush;

    public:
        Logging();
        void init();
        
        enum severity_level{
            trace,
            info,
            normal,
            notification,
            warning,
            error,
            critical
        };
        
        // logging methods
        void log_trace(const std::string& msg, const std::string& filter = "");
        void log_trace(const std::stringstream& msg, const std::string& filter = "");
        void log(severity_level sl,const std::stringstream& msg, const std::string& filter = "");
};

#endif