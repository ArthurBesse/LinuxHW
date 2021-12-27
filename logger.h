#include <iomanip>
#include <ctime>
#include <iostream>
#include <sstream>
#include <chrono>
#include <fstream>


class Logger
{
private:

        static inline Logger* this_ptr = NULL;
        std::ofstream outf;
        std::ostream& out;
public:
        Logger(char const * filename)
                : outf(filename)
                , out(filename ?
                        outf : std::cout)
        {
                if(NULL != this_ptr)
                        throw std::runtime_error("Only one instance of Logger can be created");
                this_ptr = this;
        }

        ~Logger()
        {
                outf.close();
                this_ptr = NULL;
        }

        enum severity_t{ INFO, WARNING, ERROR, FATAL, DEBUG };


        template<typename ...T>
        static void logf(Logger::severity_t severity, const char * FILE, size_t LINE, T&& ... args) //__attribute__ ((format(printf, 4, 5)))
        {
                if(nullptr == this_ptr)
                        throw std::runtime_error("The logger must first be instantiated");
                this_ptr->logf_internal(severity, FILE, LINE, args ...);
        }

private:

        char const * get_severity_str(Logger::severity_t severity)
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
        template<typename ...T>
        void logf_internal(Logger::severity_t severity, const char * FILE, size_t LINE, char const * fmt, T&& ... args)
        {
                std::time_t cur_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                std::tm * cur_time_local = std::localtime(&cur_time);
                out << std::put_time(cur_time_local, "%Y%m%d%H%M%S") << " " << get_severity_str(severity) << ": " << this->format(fmt, std::forward<T>(args)...);
        }

        template<typename ... Args>
        std::string format( char const * format, Args ... args )
        {
            int size_s = std::snprintf( nullptr, 0, format, args ... ) + 1; // Extra space for '\0'
            if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
            size_t size = static_cast<size_t>( size_s );
            auto buf = std::make_unique<char[]>( size );
            std::snprintf( buf.get(), size, format, args ... );
            return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
        }

};
