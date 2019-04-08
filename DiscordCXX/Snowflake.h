#pragma once
#include "DiscordHeaders.h"

class Snowflake
{
public:
	Snowflake() :Snowflake(0) {};
	Snowflake(uint64_t snowflake) :_snowflake(snowflake) {}
	uint64_t Timestamp() {
		(_snowflake >> 22) + 1420070400000;
	}
	uint8_t InternalWorkerId() {
		return (_snowflake & 0x3E0000) >> 17;
	}
	uint8_t InternalProcessId() {
		return (_snowflake&0x1F000)>>12;
	}
	uint16_t Increment() {
		return _snowflake & 0xFFF;
	}
	~Snowflake();
private:
	uint64_t _snowflake;

};

