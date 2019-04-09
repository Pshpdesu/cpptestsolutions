#include "DiscordCxxBot.h"


DiscordCxxBot::DiscordCxxBot(std::wstring token, std::wstring permissions)
	:_Token(token), _Permissions(permissions),_HttpClient(Discord::Constants::Paths::URIBASE)
{
	 
}

DiscordCxxBot::~DiscordCxxBot()
{
}

web::http::http_request DiscordCxxBot::GetBaseRequestBuilder(web::http::method method)
{
	web::http::http_request reqst(method);
	reqst.headers().add<std::wstring>(L"Authorization", _Token);
	reqst.headers().add<std::wstring>(L"User-Agent", L"DiscordBot/0.01");
	return std::move(reqst);
}
