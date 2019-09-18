#pragma once
#include "DiscordCredentials.h"
#include <cpprest/json.h>
#include <string>
using namespace web::json;
namespace Discord {
namespace Constants {
namespace messages_templates {
const auto payload = ([]() -> value {
    value obj(value::object());
    obj[L"op"] = value::number(0);
    obj[L"d"] = value::object();
    //obj[L"s"] = value();
    //obj[L"t"] = value();
    return obj;
    })();
const auto identification = ([]() -> value {
  value obj(value::object());
  obj[L"token"] =
      value::string(Discord::Constants::Credentials::TOKEN);
  {
    value props = value::object();
    props[L"$os"] = value::string(L"bolgen os");
    props[L"$browser"] = value::string(L"none");
    props[L"$device"] = value::string(L"none");
    obj[L"properties"] = props;
  }
  
  auto res = value(payload);
  res[L"op"] = value::number(2);
  res[L"d"] = obj;
  return res;
})();
} // namespace messages_templates
} // namespace Constants
} // namespace Discord
