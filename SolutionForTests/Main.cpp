#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#define _CRT_SECURE_NO_WARNINGS
#include "../DiscordCXX/DiscordCredentials.h"
#include "../DiscordCXX/DiscordCxxBot.h"
#include "Headers.h"

int main(int argc, char **argv) {

  DiscordCxxBot bot(Discord::Constants::Credentials::TOKEN);
  auto resp = bot.SendRequest(web::http::methods::GET, L"users/@me/guilds");
  auto a = resp.get();
  auto c = a.headers();
  auto b = a.extract_string().get();

  std::cin.get();

  return 0;
}
