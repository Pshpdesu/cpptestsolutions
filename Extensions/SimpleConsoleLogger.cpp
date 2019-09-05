#include "SimpleConsoleLogger.h"

bool loggers::simple_logger::Log(const std::string& val)
{
	return false;
}

ILogger& loggers::simple_logger::operator<<(const std::string& val)
{
	// TODO: insert return statement here
}
