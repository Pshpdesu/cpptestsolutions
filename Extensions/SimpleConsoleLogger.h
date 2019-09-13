#pragma once
#include <iostream>
#include <chrono> 
#include <ctime>
#include <iomanip>
#include <ostream>
#include "./ILogger.h"

namespace loggers {
	class simple_logger : public ILogger {
		std::ostream output;
		// Inherited via ILogger
	public:
		simple_logger()
			:output(std::cout.rdbuf())
		{
			output.imbue(std::locale("en"));
		}
		virtual bool Log(const std::string& val) override {
			/*auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());*/
			auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			output << std::put_time(std::gmtime(&now),"%c %Z") <<": " << val << std::endl;
			return true;
		}
		virtual ILogger& operator<<(const std::string& val) override {
			Log(val);
			return *this;
		}
		virtual ILogger& operator<<(const std::wstring& val) override {
			ILogger::Log(val);
			return *this;
		}
	};
}