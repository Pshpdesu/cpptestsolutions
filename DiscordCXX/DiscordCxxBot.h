#pragma once
#include "../Extensions/ConcurrentLogger.h"
#include "../Extensions/SimpleConsoleLogger.h"
#include "DiscordConstants.h"
#include "DiscordJSONTemplates.hpp"
#include <chrono>
#include <cpprest/filestream.h>
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <cpprest/streams.h>
#include <cpprest/uri.h>
#include <cpprest/uri_builder.h>
#include <cpprest/ws_client.h>
#include <cpprest/ws_msg.h>
#include <optional>
#include <string>

using namespace web;
using namespace std::chrono;

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

  ~DiscordCxxBot() {
    _wsClbckClient.close();
    _exit_flag = true;
    heartbeat_thread.join();
    (*LOGGER) << "Connection closed";
  }

private:
  const std::wstring _token = L"";
  const std::wstring _permissions = L"";
  const std::wstring _base_uri = L"";

  http::client::http_client _httpClient;
  std::optional<uint64_t> heartbeat;
  std::optional<json::value> last_s;
  std::atomic<bool> is_authorized = false;
  std::thread heartbeat_thread;

  web_sockets::client::websocket_callback_client _wsClbckClient;

  std::atomic<bool> _exit_flag;

  void ConnectToWebSocket() {
    using namespace web_sockets::client;

    auto gateway_resp =
        SendRequest(http::methods::GET, L"/gateway/bot")
            .then([](http::http_response resp) { return resp.extract_json(); })
            .then([](json::value resp) {
              (*LOGGER) << resp.serialize();
              return resp;
            })
            .get();

    websocket_client_config config;
    typedef Concurrency::streams::istream govno;
    _wsClbckClient.set_close_handler(
        [this](websocket_close_status status,
               utility::string_t reason, std::error_code error) {
          _exit_flag = true;
          *LOGGER << "Web socket was closed. Reason: " +
                         Utility::string_helpers::wstring_to_string(reason);
          *LOGGER << "Websocket close status: "+error.message();
          heartbeat_thread.join();
          std::terminate();
        });
    _wsClbckClient.set_message_handler(
        [this](const websocket_incoming_message &msg) -> void {
          msg.extract_string().then([this](const std::string str) {
            process_messages(
                json::value::parse(utility::conversions::to_string_t(str)));
          });
        });

    _wsClbckClient.connect(gateway_resp[L"url"].as_string() +
                           L"?v=6&encoding=json");
  }

  void authorization_callback(const json::value &data) {
    send_message(Discord::Constants::messages_templates::identification)
        .then([]() {
          *LOGGER << L"Message: "
                     "" +
                         Discord::Constants::messages_templates::identification
                             .serialize() +
                         L""
                         " was sent";
        });
    is_authorized = true;
  }

  void process_messages(const json::value &msg) {
    *LOGGER << msg.serialize();
    if (msg.has_field(L"s")) {
      last_s = msg.at(L"s");
    }
    if (msg.has_object_field(L"d") &&
        msg.at(L"d").has_field(L"heartbeat_interval")) {
      if (!is_authorized.load()) {
        authorization_callback(msg);
        heartbeat =
            msg.at(L"d").at(L"heartbeat_interval").as_number().to_uint64();
        heartbeat_thread = std::thread([this]() {
          heartbeat_cicle();
          *LOGGER << "Heartbeat thread closed;";
        });
        return;
      }
    }
  }

  void heartbeat_cicle() {
    while (!_exit_flag.load()) {
      std::this_thread::sleep_for(milliseconds(heartbeat.value() - 5));
      if (_exit_flag.load()) {
        return;
      }
      send_message(Discord::Constants::messages_templates::heartbeat)
          .then([]() {
            *LOGGER << "Heartbeat was sent. Heartbeat interval: NaN";
          });
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

  Concurrency::task<void> send_message(json::value message) {
    websockets::client::websocket_outgoing_message msg;
    if (message[L"op"].as_number().to_int64() == 1)
      message[L"d"] = last_s.has_value() ? last_s.value() : json::value::null();
    *LOGGER << Utility::string_helpers::wstring_to_string(message.serialize());
    msg.set_utf8_message(
        Utility::string_helpers::wstring_to_string(message.serialize()));
    return _wsClbckClient.send(msg);
  }
};
