#include "logger.h"

void logger_init() {
    logging::add_file_log(
        keywords::file_name = "log/SYSLOG_%N.log",
        keywords::rotation_size = 10 * 1024 * 1024,
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
        keywords::format = "[%TimeStamp%]:[%ThreadID%]:[%Severity%]:%Message%",
        keywords::auto_flush = true
    );

    // Log to console
    logging::add_console_log(
        std::clog,
        keywords::filter = logging::trivial::severity >= logging::trivial::warning,
        keywords::format = "[%TimeStamp%]:[%ThreadID%]:[%Severity%]:%Message%",
        keywords::auto_flush = true
    );

    logging::core::get()->set_filter(
        logging::trivial::severity >= logging::trivial::info
    );

    // This line adds common attributes such as TimeStamp and ThreadID to the log records. 
    logging::add_common_attributes();
}
