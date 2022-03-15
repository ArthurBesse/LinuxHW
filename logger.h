#include <thread>
#include <iomanip>
#include <ctime>
#include <iostream>
#include <sstream>
#include <syncstream>
#include <chrono>
#include <fstream>
#include <mutex> 
#pragma GCC diagnostic ignored "-Wformat-security"

class Logger
{
public:
	
enum severity_t { INFO, WARNING, ERROR, FATAL, DEBUG };

private:

        static inline Logger* this_ptr = nullptr;
        std::ofstream outf;
        std::ostream& out;
	bool const debug;
	bool const show_source;
	bool const colored;


public:
        Logger(char const * filename, bool debug_param = true, bool show_source_param = true, bool colored_param = true)
                : outf(filename)
		, out(filename ?
                        outf : std::cout)
		, debug(debug_param)
		, show_source(show_source_param)
		, colored(colored_param)
        {
                if(NULL != this_ptr)
                        throw std::runtime_error("Only one instance of Logger can be created");
                this_ptr = this;
        }

        ~Logger()
        {
		out << std::flush;
                outf.close();
                this_ptr = nullptr;
        }

	Logger(const Logger&) = delete;

        Logger& operator=(const Logger&) = delete;

        Logger(Logger&&) = delete;

        Logger& operator=(Logger&&) = delete;


	//TODO: Add static check for correct formatting
        template<typename ...T>
	static void logf(Logger::severity_t severity, const char * FILE, size_t LINE, char const* fmt, T&& ... args)  //__attribute__ ((format(printf, 4, 5)));
	{
        	if(nullptr == this_ptr)
                	throw std::runtime_error("The logger must first be instantiated");

	        if(Logger::DEBUG == severity && false == this_ptr->debug)
        	        return;

	        this_ptr->logf_internal(severity, FILE, LINE, fmt, std::forward<T>(args)...);
	}



private:

        char const * get_severity_str(Logger::severity_t severity) const
        {
                switch(severity)
                {
                case INFO:
                        return "INFO";
                        break;
                case WARNING:
                        return "WARNING";
                        break;
                case ERROR:
                        return "ERROR";
                        break;
		case FATAL:
                        return "FATAL";
                        break;
                case DEBUG:
                        return "DEBUG";
                }
                return nullptr;
        }

	char const * get_severity_color_str(Logger::severity_t severity) const
	{
		switch(severity)
                {
                case Logger::ERROR:
			return "\033[31m";
		case Logger::WARNING:
			return "\033[32m";
                case Logger::DEBUG:
                        return "\033[34m";
                default:
                        return "\033[39m";
                }
	}

	template<typename ...T>
        void logf_internal(Logger::severity_t severity, const char * FILE, size_t LINE, char const * fmt, T&& ... args)
        {
                std::time_t cur_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                std::tm * cur_time_local = std::localtime(&cur_time);
                if(true == show_source)
                        std::osyncstream(out) << get_severity_color_str(severity) << std::put_time(cur_time_local, "%Y%m%d%H%M%S") << " [" << std::this_thread::get_id() << "] " << get_severity_str(severity) << ": " << this->format(fmt, std::forward<T>(args)...) << " (FROM: " << FILE << ":" << LINE << ")" << get_severity_color_str(Logger::severity_t::INFO) << std::endl;
                else
                        std::osyncstream(out) << get_severity_color_str(severity) << std::put_time(cur_time_local, "%Y%m%d%H%M%S") << " [" << std::this_thread::get_id() << "] " << get_severity_str(severity) << ": " << this->format(fmt, std::forward<T>(args)...) << get_severity_color_str(Logger::severity_t::INFO) << std::endl;
        }

        template<typename ... Args>
        std::string format( char const * format, Args&& ... args )
        {
            int size_s = std::snprintf( nullptr, 0, format, args ... ) + 1; 
            if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
            size_t size = static_cast<size_t>( size_s );
            auto buf = std::make_unique<char[]>( size );
            std::snprintf( buf.get(), size, format, args ... );
            return std::string( buf.get(), buf.get() + size - 1 ); 
        }
};
