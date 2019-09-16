#pragma once
#include "DiscordHeaders.h"

#include <chrono>
#include <optional>
#include <cpprest/filestream.h>
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <cpprest/streams.h>
#include <cpprest/uri.h>
#include <cpprest/uri_builder.h>
#include <cpprest/ws_client.h>
#include <cpprest/ws_msg.h>
#include "../Extensions/ConcurrentLogger.h"
#include "../Extensions/SimpleConsoleLogger.h"
#include "DiscordConstants.h"

using namespace std::chrono_literals;
using namespace web;


std::shared_ptr<ILogger> LOGGER{std::shared_ptr<ConcurrentLogger>{
    new ConcurrentLogger{std::shared_ptr<loggers::simple_logger>{
        new loggers::simple_logger{}}}}};

std::string get_thread_id() {
  std::ostringstream ss{};
  ss << std::this_thread::get_id();
  return ss.str();
}

class DiscordCxxBot {
public:
  DiscordCxxBot(std::wstring token, std::shared_ptr<ILogger> logger,
                std::wstring permissions = L"")
      : DiscordCxxBot(token, permissions) {
    LOGGER = logger;
  }
  DiscordCxxBot(std::wstring token, std::wstring permissions = L"")
      : _token(token), _permissions(permissions),
        _base_uri(Discord::Constants::Paths::URIBASE +
                  Discord::Constants::Paths::APIBASE +
                  Discord::Constants::Paths::APIVERSION),
        _httpClient(_base_uri) {
    ConnectToWebSocket();
    (*LOGGER) << "DiscordBot started";
  }

  Concurrency::task<http::http_response> SendRequest(const http::method method,
                                                     const std::wstring path) {
    auto reqst = GetBaseRequestBuilder(method);
    reqst.set_request_uri(path);
    uri a = reqst.absolute_uri();
    return _httpClient.request(reqst);
  }

  ~DiscordCxxBot() { _wsClbckClient.close();  (*LOGGER) << "Connection closed"; }

private:
  const std::wstring _token = L"";
  const std::wstring _permissions = L"";
  const std::wstring _base_uri = L"";

  http::client::http_client _httpClient;
  std::optional<uint64_t> heartbeat;

  web_sockets::client::websocket_callback_client _wsClbckClient;

  std::atomic<bool> _exit_flag;

  void process_messages(const std::string &msg) {
    *LOGGER << msg;
    auto json_msg = json::value::parse(utility::conversions::to_utf16string(msg));
    if (json_msg.has_object_field(L"d") &&
        json_msg[L"d"].has_field(L"heartbeat_interval")) {
      auto time = json_msg[L"d"][L"heartbeat_interval"].as_number().to_uint64();

      heartbeat = time;

      json::value heartbeat;
      heartbeat[L"op"] = 11;
      websockets::client::websocket_outgoing_message msg;
      *LOGGER << (L"Hello message: " + heartbeat.serialize());
      msg.set_utf8_message(Utility::string_helpers::wstring_to_string(heartbeat.serialize()));
      _wsClbckClient.send(msg).then([](pplx::task<void> t)
          {
              try
              {
                  (*LOGGER)<<"message was sent";
              }
              catch (const web::websockets::client::websocket_exception& ex)
              {
                  std::cout << ex.what();
              }
          });
      //auto sleep_for = std::chrono::duration(std::chrono::milliseconds(time));
      //std::this_thread::sleep_for(sleep_for);
    }
  }

  void ConnectToWebSocket() {
    auto gateway_resp = SendRequest(http::methods::GET, L"/gateway/bot")
                            .then([](http::http_response resp) {
                              (*LOGGER) << "THREAD #" + get_thread_id();
                              return resp.extract_json();
                            })
                            .then([](json::value resp) {
                              (*LOGGER) << "THREAD #" + get_thread_id();
                              (*LOGGER) << resp.serialize();
                              return resp;
                            })
                            .get();
    using namespace web_sockets::client;

    websocket_client_config config;
    typedef Concurrency::streams::istream govno;
    _wsClbckClient.set_message_handler(
        [this](const websocket_incoming_message &msg) -> void {
          (*LOGGER) << "THREAD #" + get_thread_id();
          msg.extract_string().then(
              [this](const std::string str) { process_messages(str); });
        });

    _wsClbckClient.connect(gateway_resp[L"url"].as_string() +
                           L"?v=6&encoding=json");
  }

  void heartbeat_cicle() {
    std::chrono::time_point start{std::chrono::system_clock::now()};
    std::chrono::time_point stop{std::chrono::system_clock::now()};

    while (_exit_flag.load()) {
      if (!heartbeat.has_value()) {
        continue;
      }
      stop = std::chrono::system_clock::now();
      auto govno = (stop - start).count();
      if (govno >= heartbeat.value()) {
        start = stop;
        json::value heartbeat;
        heartbeat[L"op"] = 11;
        websockets::client::websocket_outgoing_message msg;
        msg.set_utf8_message(Utility::string_helpers::wstring_to_string(heartbeat.serialize()));
        _wsClbckClient.send(msg).then([]() {*LOGGER << "Hello message was sent"; });
      }
    }
  }

  http::http_request GetBaseRequestBuilder(http::method method) {
    http::http_request reqst(method);
    auto test = reqst.absolute_uri();
    reqst.headers().add<std::wstring>(L"Authorization", L"Bot " + _token);
    reqst.headers().add<std::wstring>(L"User-Agent",
                                      L"DisCxx (html://none.com, v0.1)");

    reqst._set_base_uri(_base_uri);
    reqst.headers().set_content_type(L"application/json");
    return std::move(reqst);
  }
};
