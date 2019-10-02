#pragma once
#include "../Extensions/ConcurrentLogger.h"
#include "../Extensions/SimpleConsoleLogger.h"
#include "DiscordConstants.h"
#include "DiscordJSONTemplates.hpp"
#include "DiscordUtilities.h"
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

using namespace std;
using namespace web;
using namespace chrono;
using namespace Discord::Utilities;

shared_ptr<ILogger> LOGGER{shared_ptr<ConcurrentLogger>{new ConcurrentLogger{
    shared_ptr<loggers::simple_logger>{new loggers::simple_logger{}}}}};

string get_thread_id() {
  ostringstream ss{};
  ss << this_thread::get_id();
  return ss.str();
}

class DiscordCxxBot {
public:
  DiscordCxxBot(wstring token, shared_ptr<ILogger> logger,
                wstring permissions = L"")
      : DiscordCxxBot(token){//, permissions) {
    LOGGER = logger;
  }
  DiscordCxxBot(wstring token)//, wstring permissions = L"")
      : _token(token),// _permissions(permissions),
        _base_uri(Discord::Constants::Paths::URIBASE +
                  Discord::Constants::Paths::APIBASE +
                  Discord::Constants::Paths::APIVERSION),
        _httpClient(_base_uri) {
    ConnectToWebSocket();
    (*LOGGER) << "DiscordBot started";
  }

  Concurrency::task<http::http_response> SendRequest(const http::method method,
                                                     const wstring path) {
    auto reqst = GetBaseRequestBuilder(method);
    reqst.set_request_uri(path);
    uri a = reqst.absolute_uri();
    return _httpClient.request(reqst);
  }

  ~DiscordCxxBot() {
    _wsClbckClient.close().wait();
    (*LOGGER) << "Connection closed";
  }

private:
  const wstring _token = L"";
  const wstring _permissions = L"";
  const wstring _base_uri = L"";

  http::client::http_client _httpClient;
  bool is_heartbeat_sent = false;
  optional<uint64_t> heartbeat;
  optional<json::value> last_s;
  atomic<bool> is_authorized = false;
  string session_id;
  thread heartbeat_thread;

  web_sockets::client::websocket_callback_client _wsClbckClient;

  atomic<bool> _exit_flag;

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
    _wsClbckClient.set_close_handler([this](websocket_close_status status,
                                            utility::string_t reason,
                                            error_code error) {
      _exit_flag = true;
      *LOGGER << "Web socket was closed. Reason: " +
                     Utility::string_helpers::wstring_to_string(reason);
      *LOGGER << "Websocket close status: " + error.message();
      heartbeat_thread.join();
      //terminate();
    });
    _wsClbckClient.set_message_handler(
        [this](const websocket_incoming_message &msg) -> void {
          msg.extract_string().then([this](const string str) {
            process_messages(
                json::value::parse(utility::conversions::to_string_t(str)));
          });
        });

    _wsClbckClient
        .connect(gateway_resp[L"url"].as_string() + L"?v=6&encoding=json")
        .wait();
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
        })
        .then([this]() { is_authorized = true; });
  }

  void process_messages(const json::value &msg) {
    *LOGGER << msg.serialize();
    if (msg.has_field(L"s")) {
      last_s = msg.at(L"s");
    }
    auto [type, name] = get_message_type(msg);
    switch (type) {
    case message_type::Hello:
      if (!is_authorized.load()) {
        authorization_callback(msg);
        heartbeat =
            msg.at(L"d").at(L"heartbeat_interval").as_number().to_uint64();
        heartbeat_thread = thread([this]() {
          heartbeat_cicle();
          *LOGGER << "Heartbeat thread closed;";
        });
        return;
      }
      break;
    case message_type::Unknown:
    default:
      *LOGGER << L"Unknown message:" + msg.serialize();
      break;
    }
  }

  void heartbeat_cicle() {
    while (!_exit_flag.load()) {
      this_thread::sleep_for(milliseconds(heartbeat.value()));
      if (_exit_flag.load()) {
        return;
      }
      auto msg = Discord::Constants::messages_templates::heartbeat;
      msg[L"d"] = last_s.has_value() ? last_s.value() : json::value::null();
      send_message(msg).then([this]() {
        *LOGGER << "Heartbeat was sent.";
        is_heartbeat_sent = true;
      });
    }
  }

  http::http_request GetBaseRequestBuilder(http::method method) {
    http::http_request reqst(method);
    auto test = reqst.absolute_uri();
    reqst.headers().add<wstring>(L"Authorization", L"Bot " + _token);
    reqst.headers().add<wstring>(L"User-Agent",
                                 L"DisCxx (html://none.com, v0.1)");

    reqst._set_base_uri(_base_uri);
    reqst.headers().set_content_type(L"application/json");
    return move(reqst);
  }

  Concurrency::task<void> send_message(json::value message) {
    websockets::client::websocket_outgoing_message msg;
    if (is_heartbeat_sent) {
      *LOGGER << "Heartbeat ACK was not received. Reconnecting...";
    }
    *LOGGER << Utility::string_helpers::wstring_to_string(message.serialize());
    msg.set_utf8_message(
        Utility::string_helpers::wstring_to_string(message.serialize()));
    return _wsClbckClient.send(msg);
  }
};
