#pragma once
#include <iostream>
#include <chrono> 
#include <iomanip>
#include <ostream>
#include "./ILogger.h"

namespace loggers {
	class simple_logger : ILogger {
		std::ostream output;
		// Inherited via ILogger
	public:
		simple_logger() {
			output.rdbuf(std::cout.rdbuf());
			output.imbue(std::locale("en"))
		}
		virtual bool Log(const std::string& val) override {
			auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			std::cout << std::put_time(std::localtime(&now), "%H:%M:%S: ")<<val<<std::endl;
		}
		virtual ILogger& operator<<(const std::string& val) override {
		}
	};
}