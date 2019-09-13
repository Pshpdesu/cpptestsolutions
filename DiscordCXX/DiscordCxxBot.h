#pragma once
#include "../Extensions/ConcurrentLogger.h"
#include "../Extensions/SimpleConsoleLogger.h"
#include "DiscordConstants.h"
#include <cpprest/filestream.h>
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <cpprest/streams.h>
#include <cpprest/uri.h>
#include <cpprest/uri_builder.h>
#include <cpprest/ws_client.h>
#include <cpprest/ws_msg.h>
#include <iostream>
#include <string>
#include <string_view>

std::shared_ptr<ILogger> LOGGER{std::shared_ptr<ConcurrentLogger>{
    new ConcurrentLogger{std::shared_ptr<loggers::simple_logger>{
        new loggers::simple_logger{}}}}};

std::string get_thread_id() {
  std::ostringstream ss;
  ss << std::this_thread::get_id();
  return ss.str();
}

class DiscordCxxBot {
public:
  DiscordCxxBot(std::wstring token, std::shared_ptr<ILogger> logger,
                std::wstring permissions = L"")
      : DiscordCxxBot(token, permissions) {
    LOGGER = logger;

    (*LOGGER) << "DiscordBot constructor";
  }
  DiscordCxxBot(std::wstring token, std::wstring permissions = L"")
      : _token(token), _permissions(permissions),
        _base_uri(Discord::Constants::Paths::URIBASE +
                  Discord::Constants::Paths::APIBASE +
                  Discord::Constants::Paths::APIVERSION),
        _httpClient(_base_uri) {
    ConnectToWebSocket();
  }

  Concurrency::task<web::http::http_response>
  SendRequest(const web::http::method method, const std::wstring path) {
    auto reqst = GetBaseRequestBuilder(method);
    reqst.set_request_uri(path);
    web::uri a = reqst.absolute_uri();
    return _httpClient.request(reqst);
  }

  ~DiscordCxxBot() { _wsClbckClient.close(); }

private:
  const std::wstring _token = L"";
  const std::wstring _permissions = L"";
  const std::wstring _base_uri = L"";

  web::http::client::http_client _httpClient;
  uint32_t heartbeat = -1;

  web::web_sockets::client::websocket_client _wsClient;
  web::web_sockets::client::websocket_callback_client _wsClbckClient;

  void process_messages(const std::string &msg) {
    *LOGGER << msg;
    auto json_msg = web::json::value(msg.c_str());
    if (json_msg.has_field(L"d") &&
        json_msg[L"d"].has_field(L"heartbeat_interval")) {
      auto time = json_msg[L"d"][L"heartbeat_interval"].as_number().to_int32();

      auto sleep_for = std::chrono::duration(std::chrono::milliseconds(time));
      std::this_thread::sleep_for(sleep_for);
    }
  }

  void ConnectToWebSocket() {
    auto gateway_resp = SendRequest(web::http::methods::GET, L"/gateway/bot")
                            .then([](web::http::http_response resp) {
                              (*LOGGER) << "THREAD #" + get_thread_id();
                              return resp.extract_json();
                            })
                            .then([](web::json::value resp) {
                              (*LOGGER) << "THREAD #" + get_thread_id();
                              (*LOGGER) << resp.serialize();
                              return resp;
                            })
                            .get();
    using namespace web::web_sockets::client;

    websocket_client_config config;
    typedef Concurrency::streams::istream govno;
    _wsClbckClient.set_message_handler(
        [this](const websocket_incoming_message &msg) -> void {
          (*LOGGER) << "THREAD #" + get_thread_id();
          msg.extract_string().then(
              [this](std::string str) { process_messages(str); });
        });

    //	if (logger.get() != nullptr) {
    //		*logger << "Message: \n";
    //		*logger << msg_string;
    //	}

    _wsClbckClient.connect(gateway_resp[L"url"].as_string() +
                           L"?v=6&encoding=json");
    // auto connection_res =
    // _wsClient.receive().then([this](websocket_incoming_message msg) {return
    // msg.body(); }) 	.then([this](govno body)
    //		{
    //			return web::json::value(body);
    //		}
    //).get();

    // auto hello_message =
    // web::json::value(_wsClient.receive().get().extract_string().get());
    // hello_message[L"d"][L"heartbeat_interval"];
  }

  web::http::http_request GetBaseRequestBuilder(web::http::method method) {
    web::http::http_request reqst(method);
    auto test = reqst.absolute_uri();
    reqst.headers().add<std::wstring>(L"Authorization", L"Bot " + _token);
    reqst.headers().add<std::wstring>(L"User-Agent",
                                      L"DisCxx (html://none.com, v0.1)");

    reqst._set_base_uri(_base_uri);
    reqst.headers().set_content_type(L"application/json");
    return std::move(reqst);
  }
};
