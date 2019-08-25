
#include "Headers.h"

int main(int argc, char** argv) {
	DiscordCxxBot bot(Discord::Constants::Credentials::TOKEN, Discord::Constants::Credentials::PERMISSIONS);
	auto resp = bot.SendRequest(web::http::methods::GET,L"users/@me/guilds");
	auto a = resp.get();
	auto c = a.headers();
	auto b = a.extract_string().get();
	return 0;
}
