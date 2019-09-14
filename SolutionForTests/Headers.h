#include <chrono>
#include <codecvt>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

#include "../DiscordCXX/DiscordConstants.h"
#include "../DiscordCXX/DiscordCredentials.h"
#include "../DiscordCXX/DiscordCxxBot.h"

#include "..//Extensions//ConcurrentLogger.h"
#include "..//Extensions//SimpleConsoleLogger.h"

#include <cpprest/filestream.h>
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <cpprest/streams.h>
#include <cpprest/uri.h>
#include <cpprest/uri_builder.h>
#include <cpprest/ws_client.h>
#include <cpprest/ws_msg.h>
