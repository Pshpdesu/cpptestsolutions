#pragma once
#include "UtilityHeaders.h"
#include "ILogger.h"
#include <memory>
#include <string>
#include <mutex>

class ConcurrentLogger: public ILogger
{
public:
	ConcurrentLogger(std::shared_ptr<ILogger>& logger)
		:_logger(logger)
	{
	}
	ConcurrentLogger(std::shared_ptr<ILogger>&& logger) :
		_logger(std::move(logger))
	{
	}
	ConcurrentLogger(ConcurrentLogger&)=delete;
	ConcurrentLogger(ConcurrentLogger&& logger)
		: _logger(std::move(logger._logger))
	{
	}
	virtual bool Log(const std::string& val) override
	{
		std::scoped_lock<std::mutex> lock(_mtx);
		return _logger->Log(val+"\n");
	}
	virtual ~ConcurrentLogger()
	{
	}

	virtual ILogger& operator<<(const std::string& val) override {
		Log(val);
		return *this;
	};
private:
	std::mutex _mtx;
	std::shared_ptr<ILogger> _logger;
};

