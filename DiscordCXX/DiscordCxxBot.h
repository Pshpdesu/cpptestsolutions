#pragma once
#include "DiscordHeaders.h"

class DiscordCxxBot
{
public:
	DiscordCxxBot();
	virtual ~DiscordCxxBot();
private:
	const std::wstring CLIENT_ID = L"";
	const std::wstring CLIENT_SECRET = L"";

	const std::wstring TOKEN = L"";
	const std::wstring PERMISSIONS = L"";
	const std::wstring URIBASE = L"https://discordapp.com/";
	const std::wstring APIBASE = L"api/";
	const std::wstring OAUTHBASE = L"oauth2/";
};

