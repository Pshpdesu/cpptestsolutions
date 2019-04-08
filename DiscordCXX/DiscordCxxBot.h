#pragma once
#include "DiscordHeaders.h"

using namespace web::http::client;

class DiscordCxxBot
{
public:
	DiscordCxxBot(std::wstring token, std::wstring permissions);

	virtual ~DiscordCxxBot();
private:
	const std::wstring _Token = L"";
	const std::wstring _Permissions = L"";
	http_client _HttpClient;
	
	web::http::http_request GetBaseRequestBuilder(web::http::method method);
	
};

