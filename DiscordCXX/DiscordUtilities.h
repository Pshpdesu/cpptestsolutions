#pragma once
#include <cpprest\json.h>

namespace Discord {
namespace Utilities {
enum message_type {
  // Dispatch
  // Receive
  // dispatches an event
  Dispatch = 0,

  // Heartbeat
  // Send/Receive
  // used for ping checking
  Heartbeat = 1,

  // Identify
  // Send
  // used for client handshake
  Identify = 2,

  // Status Update
  // Send
  // used to update the client status
  Status = 3,

  // Voice State Update
  // Send
  // used to join/move/leave voice channels
  Voice = 4,

  // Resume
  // Send
  // used to resume a closed connection
  Resume = 6,

  // Reconnect
  // Receive
  // used to tell clients to reconnect to the gateway
  Reconnect = 7,

  // Request Guild Members
  // Send
  // used to request guild members
  Request = 8,

  // Invalid Session
  // Receive
  // used to notify client they have an invalid session id
  Invalid = 9,

  // Hello
  // Receive
  // sent immediately after connecting, contains heartbeat and server debug
  // information
  Hello = 10,

  // Heartbeat ACK
  // Receive
  // sent immediately following a client heartbeat that was received
  HeartbeatACK = 11,

  // Unkown message
  Unknown = -1
};
std::tuple<message_type, std::string> get_message_type(web::json::value msg) {
  std::tuple res = {message_type::Unknown, "Unknown"};
  if (!msg.has_field(L"op") && msg.at(L"op").is_number() &&
      msg.at(L"op").as_number().is_int32()) {
    return res;
  }

  switch (static_cast<message_type>(msg.at(L"op").as_number().to_int32())) {
  case message_type::Dispatch:
    return {message_type::Dispatch, "Dispatch"};
    break;
  case message_type::Heartbeat:
    return {message_type::Heartbeat, "Heartbeat"};
    break;
  case message_type::Identify:
    return {message_type::Identify, "Identify"};
    break;
  case message_type::Status:
    return {message_type::Status, "Status"};
    break;
  case message_type::Voice:
    return {message_type::Voice, "Voice"};
    break;
  case message_type::Resume:
    return {message_type::Resume, "Resume"};
    break;
  case message_type::Reconnect:
    return {message_type::Reconnect, "Reconnect"};
    break;
  case message_type::Request:
    return {message_type::Request, "Request"};
    break;
  case message_type::Invalid:
    return {message_type::Invalid, "Invalid"};
    break;
  case message_type::Hello:
    return {message_type::Hello, "Hello"};
    break;
  case message_type::HeartbeatACK:
    return {message_type::HeartbeatACK, "HeartbeatACK"};
    break;
  default:
    return res;
    break;
  }
}
enum close_codes {
  // We're not sure what went wrong. Try reconnecting?
  unknown_error = 4000,
  // You sent an invalid Gateway opcode or an invalid payload for an
  // opcode. Don't do that!
  unknown_opcode = 4001,
  // You sent an invalid payload to us.Don't do that!
  decode_error = 4002,
  // You sent us a payload prior to identifying.
  not_authenticated = 4003,
  // The account token sent with your identify payload is incorrect.
  authentication_failed = 4004,
  // You sent more than one identify payload. Don't do that!
  already_authenticated = 4005,
  // The sequence sent when resuming the session was invalid.Reconnect and start
  // a new session.
  invalid_seq = 4007,
  // Woah nelly!You're sending payloads to us too quickly. Slow it down!
  rate_limited = 4008,
  // Your session timed out. Reconnect and start a new one.
  session_timeout = 4009,
  // You sent us an invalid shard when identifying.
  invalid_shard = 4010,
  // The session would have handled too many guilds - you are required to
  // shard your connection in order to connect.
  sharding_required = 4011
};

} // namespace Utilities
} // namespace Discord
