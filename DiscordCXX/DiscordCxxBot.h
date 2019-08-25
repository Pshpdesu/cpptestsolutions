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
		_base_uri(Discord::Constants::Paths::URIBASE
			+ Discord::Constants::Paths::APIBASE
			+ Discord::Constants::Paths::APIVERSION),
		_httpClient(_base_uri)
	{
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

	~DiscordCxxBot() {}

private:
	const std::wstring _token = L"";
	const std::wstring _permissions = L"";
	const std::wstring _base_uri = L"";
	web::http::client::http_client _httpClient;
	uint32_t heartbeat = -1;

	web::web_sockets::client::websocket_client _wsClient;
	void ConnectToWebSocket() {
		auto gateway_resp = SendRequest(web::http::methods::GET, L"/gateway/bot").get();
		auto gateway_endpoint = gateway_resp.extract_json().get();
		using namespace web::web_sockets::client;


		websocket_client_config config;
		_wsClient.connect(gateway_endpoint[L"url"].as_string() + L"?v=6&encoding=json")
			.then([]());
		//auto hello_message = web::json::value(_wsClient.receive().get().extract_string().get());
		//hello_message[L"d"][L"heartbeat_interval"];
	}

	web::http::http_request GetBaseRequestBuilder(web::http::method method)
	{
		web::http::http_request reqst(method);
		auto test = reqst.absolute_uri();
		reqst.headers().add<std::wstring>(L"Authorization", L"Bot " + _token);
		reqst.headers().add<std::wstring>(L"User-Agent", L"DisCxx (html://none.com, v0.1)");

		reqst._set_base_uri(_base_uri);
		reqst.headers().set_content_type(L"application/json");
		return std::move(reqst);
	}

};

