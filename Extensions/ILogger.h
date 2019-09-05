#pragma once
class ILogger
{
public:
	virtual bool Log(const std::string& val) = 0;
	virtual ILogger& operator << (const std::string& val) = 0;
protected:
	~ILogger() {}
};