#include <iostream>
#include <sstream>
#include <initializer_list>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>


class Logger
{
private:
	static inline Logger* this_ptr = NULL;
public:
	Logger(char const * filename)
	{
		if(NULL != this_ptr)
			throw std::runtime_error("Only one instance of Logger can be created");
		this_ptr = this;
		if(NULL != filename)
			boost::log::keywords::file_name = filename;
		boost::log::keywords::format = "[%TimeStamp%] [%SEVERITY%]: %Message%";
	}

	~Logger()
	{
		this_ptr = NULL;
	}

	enum severity_t{ INFO, WARNING, ERROR, FATAL, DEBUG };
	
	template<typename ...T>
	static void log(Logger::severity_t severity, T&& ... args)
	{
		if(NULL == this_ptr)
			throw std::runtime_error("The logger must first be instantiated");
		
		std::stringstream message_constructor;
		
		std::initializer_list<int> temp{(message_constructor << std::forward<T>(args), 0) ... };

		switch(severity)
		{
		case INFO:
			BOOST_LOG_TRIVIAL(info) << message_constructor.str();
			break;
		case WARNING:
                        BOOST_LOG_TRIVIAL(warning) << message_constructor.str();
			break;
		case ERROR:
                        BOOST_LOG_TRIVIAL(error) << message_constructor.str();
			break;
		case FATAL:
                        BOOST_LOG_TRIVIAL(fatal) << message_constructor.str();
			break;
		case DEBUG:
			BOOST_LOG_TRIVIAL(debug) << message_constructor.str();
		}
	}
	template<typename ...T>
	static void logf(Logger::severity_t severity, const char * FILE, size_t LINE, T&& ... args)
        {
		log(severity, "In ", FILE, ":", LINE, " ", std::forward<T>(args) ...); 
        }

};
