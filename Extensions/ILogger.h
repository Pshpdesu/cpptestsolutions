#pragma once
#include <string>
#include <locale>
#include <codecvt>
class ILogger
{
public:
	virtual bool Log(const std::string& val) = 0;

	virtual bool Log(const std::wstring& val) final {
		return Log(wstring_to_string(val));
	};
	virtual ILogger& operator << (const std::string& val) = 0;
	virtual ILogger& operator <<(const std::wstring& val) = 0;
	virtual ~ILogger() {};
protected:
	virtual std::string wstring_to_string(const std::wstring& val) final 
	{
		std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;

		return cv.to_bytes(val);
	}; 
	//virtual std::wstring string_to_wstring(const std::string& val) final
	//{
	//	
	//};
};