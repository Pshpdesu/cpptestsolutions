#pragma once
#include "UtilityHeaders.h"
#include "ILogger.h"
#include "utility.h"

using namespace Utility::string_helpers;

class FileLogger :
	public ILogger
{
public:
	FileLogger(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path.parent_path()))
			std::filesystem::create_directory(path.parent_path());
		if (path.has_filename())
			fs = std::fstream(path.string(), std::fstream::out | std::fstream::app);
		if (!fs.is_open())
			throw std::exception(("can't open log-file " + path.generic_string()).c_str());
	}

	FileLogger(FileLogger& logger) = delete;

	FileLogger(FileLogger &&logger)
	: fs(std::move(logger.fs))
	{
	}

	virtual bool Log(const std::string& val) override
	{
		fs << val;
		return fs.good();
	}

	virtual ~FileLogger()
	{
		fs.close();
	}

	// Inherited via ILogger
	virtual ILogger& operator<<(const std::string& val) override
	{
		Log(val);
		return *this;
	
	}
	virtual ILogger& operator<<(const std::wstring& val) override
	{
		Log(val);
		return *this;

	}
	//virtual std::ostream& operator<<(std::ostream& os) override {
	//	fs << os.rdbuf();
	//	return os;
	//};
private:
	std::fstream fs;

};
