#pragma once
#include <iostream>
#include <string>
#include <string_view>
#include <cpprest/http_client.h>
#include <cpprest/ws_client.h>
#include <cpprest/ws_msg.h>
#include <cpprest/uri.h>
#include <cpprest/uri_builder.h>
#include <cpprest/json.h>
#include <cpprest/streams.h>
#include <cpprest/filestream.h>
#include "DiscordConstants.h"

class DiscordCxxBot
{
public:
	DiscordCxxBot(std::wstring token, std::wstring permissions)
		:_token(token), _permissions(permissions),
		_httpClient(Discord::Constants::Paths::URIBASE) {
		ConnectToWebSocket();
	}

	Concurrency::task<web::http::http_response> SendRequest(
		const web::http::method method, const std::wstring path)
	{
		auto reqst = GetBaseRequestBuilder(method);
		reqst.set_request_uri(path);
		web::uri a = reqst.absolute_uri();
		return _httpClient.request(reqst);
	}

	~DiscordCxxBot(){}

private:
	const std::wstring _token = L"";
	const std::wstring _permissions = L"";
	web::http::client::http_client _httpClient;
	
	web::web_sockets::client::websocket_client _wsClient;
	void ConnectToWebSocket() {
		auto gateway = SendRequest(web::http::methods::GET, L"/gateway/bot").get();
		auto a = gateway.extract_string().get();
		using namespace web::web_sockets::client;
		web::json::object msg();

		websocket_client_config config;
		_wsClient = websocket_client();
	}

	web::http::http_request GetBaseRequestBuilder(web::http::method method)
	{
		web::http::http_request reqst(method);
		reqst.headers().add<std::wstring>(L"Authorization", _token);
		reqst.headers().add<std::wstring>(L"User-Agent", L"DisCxx (html://none.com, v0.1)");
		
		reqst._set_base_uri(Discord::Constants::Paths::URIBASE);
		reqst.headers().set_content_type(L"application/json");
		return std::move(reqst);
	}
	
};

